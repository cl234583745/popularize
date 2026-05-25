嵌入式科普(54) 从 strerror 内存泄漏看错误码统一的架构问题
===
[toc]

## 引言

一段看似普通的 C 代码，只有两行，却同时暴露了**内存泄漏**和**错误码体系混乱**两个问题。而后者，是 AI 也无法替你解决的架构设计问题。

---

## 一、问题代码：两行引发的双重Bug

```c
char* GetErrorMessage(int error_number) {
    char *error_message = CipCalloc(...);          // 分配堆内存
    error_message = strerror(error_number);         // 指针被覆盖，指向静态内存
    return error_message;
}
```

代码的 TODO 注释也表达了作者的困惑：

```c
/* TODO: errno is provided by lwIP, but strerror() function is provided
   by toolchain, Is this OK? */
```

翻译：**lwIP 提供的 errno，能直接传给工具链的 strerror() 吗？**

---

## 二、根因分析：内存泄漏的本质

### 2.1 指针指向了不该指的地方

```c
char *error_message = CipCalloc(...);     // 步骤1：指针 → 堆内存块A
error_message = strerror(error_number);   // 步骤2：指针 → 静态内存块B
return error_message;                     // 步骤3：返回B的地址
```

内存状态变化：

```
步骤1 之后：
error_message ──────► [堆内存块 A]   （CipCalloc 分配的）

步骤2 之后：
error_message ──────► [静态内存块 B]  （strerror 的内部缓冲区）
                      [堆内存块 A]  ← 无人指向！内存泄漏！
```

**核心问题：strerror() 返回的指针指向库内部的静态缓冲区，不是堆内存，不能 free 也不需要 free。**

### 2.2 strerror() 内部到底返回什么

```c
// glibc 内部的简化实现
// 旧版：全局静态缓冲区（多线程不安全）
static char buf[1024];

// 新版：线程局部存储（同线程安全，跨线程互不影响）
static __thread char buf[1024];

char *strerror(int errnum) {
    // 根据 errnum 填充 buf
    return buf;                  // 返回静态/TLS内存地址，不是堆内存
}
```

| 特性 | malloc() 返回的指针 | strerror() 返回的指针 |
|---|---|---|
| 内存位置 | 堆（heap） | 静态数据段 / TLS（线程局部存储） |
| 谁管理 | 程序员 | 系统内部 |
| 需要 free？ | ✅ 必须 | ❌ 绝对不能 |
| 生命周期 | 直到 free | 程序运行期间 |
| 多次调用 | 每次返回不同内存块 | 同线程安全，跨线程可能覆盖 |

### 2.3 同样返回静态指针的 C 库函数一览

> **使用口诀**：时间错误分割线，环境区域临时名——统统返回静态区，不用释放要复制。

| 头文件 | 函数 | 返回值 | 说明 |
|---|---|---|---|
| `<string.h>` | `strerror()` | `char*` | 错误码→字符串 |
| `<string.h>` | `strtok()` | `char*` | 分割字符串，内部保存状态 |
| `<time.h>` | `asctime()` | `char*` | tm结构→字符串 |
| `<time.h>` | `ctime()` | `char*` | time_t→字符串 |
| `<stdio.h>` | `tmpnam()` | `char*` | 生成临时文件名 |
| `<stdlib.h>` | `getenv()` | `char*` | 获取环境变量 |
| `<locale.h>` | `setlocale()` | `char*` | 返回当前区域设置 |

**使用原则**：如果需要保存结果，立刻复制到自己分配的缓冲区中。

### 2.4 正确的修复方式

**方案 A：返回 const 字符串（简单场景）**

```c
const char* GetErrorMessage(int error_number) {
    return strerror(error_number);   // 直接返回，无需 free
}
```

**方案 B：调用者提供缓冲区（最清晰，推荐）**

```c
int GetErrorMessage(int error_number, char* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) return -1;
#ifdef _GNU_SOURCE
    // GNU 版本 strerror_r 返回 char*，可能不写入 buffer
    char* result = strerror_r(error_number, buffer, buffer_size);
    if (result != buffer)
        snprintf(buffer, buffer_size, "%s", result);
#else
    // POSIX 版本 strerror_r 返回 int（0表示成功）
    if (strerror_r(error_number, buffer, buffer_size) != 0)
        snprintf(buffer, buffer_size, "Unknown error: %d", error_number);
#endif
    return 0;
}

// 使用：
char msg[256];
if (GetErrorMessage(errno, msg, sizeof(msg)) == 0) {
    printf("%s\n", msg);             // 无需 free
}
```

> **注意**：`strerror_r` 存在 GNU 和 POSIX 两个版本，行为不同。GNU 版本（`_GNU_SOURCE` 定义时）返回 `char*`，POSIX 版本返回 `int`。嵌入式 Linux 常用 glibc（GNU 版本），需用条件编译处理。

**方案 C：分配堆内存 + 复制（保持原接口风格）**

```c
char* GetErrorMessage(int error_number) {
    char *error_message = CipCalloc(BUFFER_SIZE, sizeof(char));
    if (error_message == NULL) return NULL;

    char *sys_msg = strerror(error_number);
    strncpy(error_message, sys_msg, BUFFER_SIZE - 1);
    error_message[BUFFER_SIZE - 1] = '\0';
    return error_message;            // 调用者负责 free
}
```

---

## 三、更深的问题：错误码体系不统一

内存泄漏修好了，但 TODO 注释指向的那个问题才是真正的架构隐患。

### 3.1 strerror() 只认识标准 errno

C 标准只定义了 **3 个**错误码：

| 错误码 | 含义 | 典型场景 |
|---|---|---|
| EDOM | 数学参数定义域错误 | `sqrt(-1)` |
| ERANGE | 结果超出范围 | `strtol("超大数字")` |
| EILSEQ | 非法字节序列 | 字符编码转换 |

POSIX 扩展到 100+ 个（ENOMEM、EACCES、ENOENT 等），但**第三方模块的错误码 strerror 根本不认识**。

### 3.2 多模块错误码冲突实例

一个 FreeRTOS + lwIP + CherryUSB 的嵌入式系统中：

| 模块 | 错误码范围 | 定义方式 | strerror() 结果 |
|---|---|---|---|
| FreeRTOS | 0/1（pdPASS/pdFAIL） | projdefs.h | `strerror(0)` → "Success"（误导！） |
| lwIP | 负数（ERR_MEM = -1） | err.h | `strerror(-1)` → "Unknown error" |
| CherryUSB | 负数（USB_ERR_TIMEOUT = -5） | usb_errno.h | `strerror(-5)` → 乱码 |
| 自定义驱动 | 任意值 | 自己定义 | 完全不可预测 |

**根本原因**：每个模块的错误码是私有协议，不是统一标准。strerror() 只是标准 C 库为系统 errno 提供的翻译服务，不是通用错误码翻译器。

### 3.3 正确的多模块错误处理

**方案 1：各模块自提供转换函数**

```c
// lwIP 自带
const char* lwip_strerr(err_t err);

// FreeRTOS / CherryUSB：自己写查表
const char* freertos_strerror(BaseType_t err);
const char* usb_strerror(int err);
```

**方案 2：统一错误码封装层**

```c
typedef enum {
    UNIFIED_SUCCESS = 0,
    UNIFIED_FREERTOS_BASE = 0x1000,     // FreeRTOS 错误偏移
    UNIFIED_LWIP_BASE     = 0x2000,     // lwIP 错误偏移
    UNIFIED_USB_BASE      = 0x3000,     // CherryUSB 错误偏移
} unified_err_t;

const char* unified_strerror(unified_err_t err) {
    if (err >= UNIFIED_LWIP_BASE)
        return lwip_strerr((err_t)(err - UNIFIED_LWIP_BASE));
    // ... 其他模块
}
```

---

## 四、各协议栈/OS 的错误码统一现状

### 4.1 三大 RTOS 对比

| 对比维度 | RT-Thread | Zephyr | FreeRTOS |
|---|---|---|---|
| 错误码定义 | 独立定义（RT_EIO） | 兼容 Linux（EIO） | 极简（pdPASS/pdFAIL） |
| 线程本地存储 | `rt_get_errno()` | `errno` 宏（TLS） | ❌ 无 |
| 错误转字符串 | ❌ 框架不提供 | ✅ 模块化提供 | ❌ 不提供 |
| 实现方式 | — | X-Macro / 手动查表 | — |
| 可配置性 | — | ✅ Kconfig 开关 | — |
| 日志集成 | 基础 | ✅ 自动打印错误描述 | 基础 |
| 设计哲学 | 轻量精简 | 模块化、可裁剪 | 极简、最小核心 |

### 4.2 Zephyr 的模块化方案（最成熟）

Zephyr 不提供统一的 strerror，而是**每个子模块各自管理**，通过 Kconfig 控制是否包含字符串表：

```c
// 蓝牙模块
bt_security_err_to_str(err)   // 需开启 CONFIG_BT_SECURITY_ERR_TO_STR=y
// 蓝牙配对
bt_smp_err_to_str(err)
// 蓝牙属性协议
bt_att_err_to_str(err)
```

未开启时函数返回空字符串 `""`，不占 Flash 空间。日志系统自动集成：

```
// 开启字符串功能时：
Security failed: err 6 BT_SECURITY_ERR_PAIR_NOT_ALLOWED
// 关闭时（节省Flash）：
Security failed: err (6)
```

### 4.3 RT-Thread：有 Kconfig 但不用于错误字符串

RT-Thread 的 Kconfig/menuconfig 用于功能裁剪（如 `RT_USING_FINSH`、`BSP_USING_UART1`），**但没有提供 `*_ERR_TO_STR` 类选项**。设计哲学是极致轻量，错误字符串留给开发者按需实现。

### 4.4 主流通信协议栈的错误处理

| 模块 | 自带 strerror | 函数名 | 设计特点 |
|---|---|---|---|
| lwIP | ✅ | `lwip_strerr(err)` | 统一 err_t 类型 |
| PJSIP | ✅ | `pj_strerror(statcode, buf, size)` | 返回 pj_str_t，可注册自定义错误空间 |
| FreeRTOS | ❌ | — | 只有 pdPASS/pdFAIL |
| CherryUSB | ❌ | — | 需自行实现 |
| Zephyr OS | ✅ | `bt_err_to_str()` 等 | 模块化、Kconfig 可裁剪 |

---

## 五、错误码转字符串的核心实现模式

### 5.1 技术层级

```
┌─────────────────────────────────┐
│   应用层：直接使用或封装          │
├─────────────────────────────────┤
│  X-Macro 技术（代码生成技巧）     │
├─────────────────────────────────┤
│   查表法（核心算法）              │
│   - 数组索引 O(1)                │
│   - 线性查找 O(n)                │
│   - 二分查找 O(log n)            │
├─────────────────────────────────┤
│   底层：常量字符串存储            │
└─────────────────────────────────┘
```

**查表法 = 核心思想**（将错误码映射到字符串）
**X-Macro = 实现技巧**（让表更容易维护，只需维护一处）

### 5.2 X-Macro 实现（推荐）

```c
// 只维护这一个表 ✅
#define ERROR_TABLE \
    X(0,  RT_EOK,      "Success") \
    X(1,  RT_ERROR,    "Error") \
    X(2,  RT_ETIMEOUT, "Timeout") \
    X(5,  RT_ENOMEM,   "Out of memory")

// 自动生成枚举
#define X(code, name, str) name = code,
typedef enum { ERROR_TABLE } rt_err_t;
#undef X

// 查找函数（switch-case，支持非连续错误码）
const char* rt_strerror(rt_err_t err) {
    switch (err) {
#define X(code, name, str) case code: return str;
        ERROR_TABLE
#undef X
        default: return "Unknown error";
    }
}
```

> **提示**：如果错误码是连续的（0,1,2,3...），可以用数组索引 `O(1)` 查找。如果错误码不连续（如上例中 0,1,2,5），用 switch-case 由编译器优化为跳转表，效率同样很高。

### 5.3 场景选择

| 场景 | 推荐方案 |
|---|---|
| 通用 Linux/Unix 程序 | `strerror_r`（标准库自带） |
| 嵌入式 RTOS/MCU | X-Macro 查表法（不依赖工具链） |
| 多模块集成系统 | 统一错误码封装层 |
| 大型框架开发 | 自研框架（如 PJSIP 的 pj_strerror） |
| 调试阶段 | 完整字符串表 |
| 发布阶段 | Kconfig/宏开关去掉字符串节省 Flash |

---

## 六、总结：架构问题不是AI写出来的Bug，是人要教给AI的知识

### 回顾这条问题链

```
strerror 返回静态指针
    ↓
指针覆盖 → 内存泄漏
    ↓
strerror 只认识标准 errno
    ↓
各模块错误码互不兼容
    ↓
嵌入式多模块系统缺乏统一错误码架构
```

内存泄漏是**代码级Bug**，AI 能识别和修复。但错误码体系不统一是**架构级问题**——这不是 AI 写代码时犯的错，而是**人在扩展知识时发现的问题**。这类问题，AI 自己不会意识到，需要人来告诉它。

### 人与AI的认知边界

| 维度 | AI 能独立完成 | 需要人告诉AI |
|---|---|---|
| 语法错误 | ✅ 已基本消除 | — |
| 内存泄漏 | ✅ 能识别和修复 | — |
| 单函数Bug | ✅ 能分析根因 | — |
| 错误码设计 | — | ❌ 需要人传授业务领域知识 |
| 多模块集成策略 | — | ❌ 需要人定义资源/性能/可维护性权衡 |
| 统一架构决策 | — | ❌ 需要人传递团队共识和演进方向 |

**错误码统一**这类问题，本质是**系统架构设计**问题：FreeRTOS 选择极简、Zephyr 选择模块化、RT-Thread 选择轻量、PJSIP 选择折叠统一——每个选择背后都是对场景、资源、生态的权衡。

AI 编程虽然不再会出现语法错误了，但这类架构设计依然需要人去告诉AI。
