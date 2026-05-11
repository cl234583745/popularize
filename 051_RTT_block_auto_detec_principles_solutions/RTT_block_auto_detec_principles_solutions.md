
嵌入式科普(51)SEGGER RTT自动搜索机制与解决方案
===
[toc]

# 一、概述
- RTT控制块自动搜索在部分芯片上经常失败，需要手动输入地址，非常麻烦
- 通过分析自动探测原理，发现根本原因是RTT控制块不在J-Link数据库的RAM范围内
- 测试了4个代表性芯片工程，验证解决方案的有效性
- 嵌入式科普(52)Segger-RTT-Assistant v1.4.2完美解决自动搜索失败问题

# 二、测试环境

## 2.1 测试工程

| 工程 | 芯片 | 内核 | 内存架构 | 链接文件 |
| :--- | :--- | :--- | :--- | :--- |
| adc_ek_ra6m5_ep | RA6M5 | Cortex-M33 @ 200MHz | Flash + 单SRAM | fsp_rtt.ld |
| adc_ek_ra8d1_ep | RA8D1 | Cortex-M85 | MRAM + TCM + SRAM | fsp_rtt.ld |
| adc_ek_ra8p1_ep | RA8P1 | Cortex-M85 | SRAM0/1/2 + TCM | fsp.lld (LLVM) |
| rzn2l_rtt_print | RZN2L | Cortex-R52 | TCM + SRAM | fsp_xspi0_boot_rtt.ld |

## 2.2 测试工具

| 项目 | 参数 |
| :--- | :--- |
| J-Link 版本 | V7.94 / V9.38a |
| RTT 版本 | 8.60 |
| 工具链 | GCC 13.3.1 / LLVM (RA8P1) |

# 三、SEGGER RTT 简介

## 3.1 什么是 RTT

SEGGER RTT 是一种嵌入式实时调试输出技术，通过 J-Link 调试器实现目标芯片与主机之间的高速双向通信。

- 核心特性

| 特性 | 说明 |
| :--- | :--- |
| 实时性 | 不停止 CPU，通过 AHB-AP 后台访问内存 |
| 高速度 | 平均 1 微秒输出一行文本 |
| 低开销 | 仅需 ~500 字节 ROM + 24 字节 RAM |
| 双向通信 | 支持上行（目标→主机）和下行（主机→目标） |

- 与其他调试方式对比

| 调试方式 | 是否停止 CPU | 速度 | 实时性 |
| :--- | :--- | :--- | :--- |
| printf (UART) | 否（但阻塞） | 慢 | 差 |
| Semihosting | 是 | 很慢 | 很差 |
| SWO | 否 | 中等 | 好 |
| **RTT (AHB-AP)** | **否** | **很快** | **很好** |

## 3.2 AHB-AP 机制

**AHB-AP = Advanced High-performance Bus Access Port**（高级高性能总线访问端口）

```
┌─────────────────────────────────────────────────────────────────┐
│                        ARM Cortex-M 芯片                        │
├─────────────────────────────────────────────────────────────────┤
│   ┌─────────┐      ┌─────────┐      ┌─────────┐                │
│   │  CPU    │      │  DMA    │      │ J-Link  │                │
│   │ (内核)  │      │         │      │ (调试器) │                │
│   └────┬────┘      └────┬────┘      └────┬────┘                │
│        │                │                │ AHB-AP               │
│        ▼                ▼                ▼                      │
│   ┌─────────────────────────────────────────────┐              │
│   │              AHB 总线                        │              │
│   └─────────────────────────────────────────────┘              │
│        │                │                │                      │
│        ▼                ▼                ▼                      │
│   ┌─────────┐      ┌─────────┐      ┌─────────┐                │
│   │  SRAM   │      │  Flash  │      │  外设   │                │
│   └─────────┘      └─────────┘      └─────────┘                │
└─────────────────────────────────────────────────────────────────┘
```

- J-Link 通过 AHB-AP 读写内存，不需要停止 CPU
- 类似 DMA，有独立的访问路径
- Cortex-M 总是内置 AHB-AP，无需额外配置

## 3.3 RTT 数据结构

```c
typedef struct {
    char                    acID[16];                    // "SEGGER RTT" 标识
    int                     MaxNumUpBuffers;             // 上行缓冲区数量
    int                     MaxNumDownBuffers;           // 下行缓冲区数量
    SEGGER_RTT_BUFFER_UP    aUp[MAX_NUM_UP_BUFFERS];    // 上行缓冲区描述
    SEGGER_RTT_BUFFER_DOWN  aDown[MAX_NUM_DOWN_BUFFERS];// 下行缓冲区描述
} SEGGER_RTT_CB;
```

# 四、J-Link 自动搜索机制

## 4.1 搜索流程

J-Link RTT Viewer 自动搜索 RTT 控制块的流程：

```
步骤1: 检查向量表 0x20 位置
       │
       ├─ 有有效地址（bit[0]=1）→ 直接使用，搜索结束
       │
       └─ 没有或无效 → 进入步骤2
              │
              ▼
步骤2: 搜索 RAM 区域
       │
       └─ 在 J-Link 设备数据库配置的 RAM 范围内搜索 "SEGGER RTT" 字符串
```

## 4.2 向量表方案
```
LOG: J-Link RTT Viewer V9.38a: Logging started.
LOG: Terminal 0 added.
LOG: Terminal 1 added.
LOG: Terminal 2 added.
LOG: Connecting to J-Link via USB...
LOG: Device "R7KA8P1KF_CPU0" selected.
LOG: ConfigTargetSettings() start
LOG: Configuring FlashDLNoRMWThreshold=0x200 in order to make sure that option bytes programming is done via read-modify-write
LOG: ConfigTargetSettings() end - Took 23us
LOG: InitTarget() start
LOG: Identifying target device...
LOG: SWD selected. Executing JTAG -> SWD switching sequence...
LOG: Initializing DAP...
LOG: DAP initialized successfully.
LOG: Determining TrustZone configuration...
LOG:   Secure Debug: Enabled (SSD)
LOG: Determining currently configured transfer type by reading the AHB-AP CSW register.
LOG:   --> Correct transfer type configured. Done.
LOG: InitTarget() end - Took 36.0ms
LOG: Found SW-DP with ID 0x6BA02477
LOG: DPIDR: 0x6BA02477
LOG: CoreSight SoC-400 or earlier
LOG: Scanning AP map to find all available APs
LOG: AP[2]: Stopped AP scan as end of AP map seems to be reached
LOG: AP[0]: AHB-AP (IDR: 0x84770001, ADDR: 0x00000000)
LOG: AP[1]: APB-AP (IDR: 0x54770002, ADDR: 0x01000000)
LOG: Iterating through AP map to find AHB-AP to use
LOG: AP[0]: Core found
LOG: AP[0]: AHB-AP ROM base: 0xE00FE000
LOG: CPUID register: 0x411FD231. Implementer code: 0x41 (ARM)
LOG: Feature set: Mainline
LOG: Cache: L1 I/D-cache present
LOG: Found Cortex-M85 r1p1, Little endian.
LOG: FPUnit: 8 code (BP) slots and 0 literal slots
LOG: Security extension: implemented
LOG: Secure debug: enabled
LOG: PACBTI extension: implemented
LOG: CoreSight components:
LOG: ROMTbl[0] @ E00FE000
LOG: [0][0]: E00FF000 CID B105100D PID 000BB4D4 ROM Table
LOG: ROMTbl[1] @ E00FF000
LOG: [1][0]: E000E000 CID B105900D PID 000BBD23 DEVARCH 47702A04 DEVTYPE 00 ???
LOG: [1][1]: E0001000 CID B105900D PID 000BBD23 DEVARCH 47711A02 DEVTYPE 00 DWT
LOG: [1][2]: E0002000 CID B105900D PID 000BBD23 DEVARCH 47701A03 DEVTYPE 00 FPB
LOG: [1][3]: E0000000 CID B105900D PID 000BBD23 DEVARCH 47701A01 DEVTYPE 43 ITM
LOG: [1][5]: E0041000 CID B105900D PID 004BBD23 DEVARCH 47754A13 DEVTYPE 13 ETM
LOG: [1][6]: E0003000 CID B105900D PID 000BBD23 DEVARCH 47700A06 DEVTYPE 16 ???
LOG: [1][7]: E0042000 CID B105900D PID 000BBD23 DEVARCH 47701A14 DEVTYPE 14 CSS600-CTI
LOG: [0][1]: E0040000 CID 05900D00 PID 0BBD23B1 ???
LOG: I-Cache L1: 16 KB, 256 Sets, 32 Bytes/Line, 2-Way
LOG: D-Cache L1: 16 KB, 128 Sets, 32 Bytes/Line, 4-Way
LOG: RTT Viewer connected.
LOG: Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD
LOG: RTT: Failed to find RTT control block in vector table, trying to find it in RAM.
LOG: RTT: Search ranges: 0x22060000 - 0x22070000 
LOG: Periodic RTT: Looking for RTT CB @ 0x22060000 (Range 1/1)
LOG: RTT: Failed to find RTT control block in RAM.
LOG: Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD
LOG: RTT: Failed to find RTT control block in vector table, trying to find it in RAM.
LOG: RTT: Search ranges: 0x22060000 - 0x22070000 
LOG: Periodic RTT: Looking for RTT CB @ 0x22061000 (Range 1/1)
LOG: RTT: Failed to find RTT control block in RAM.
LOG: Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD
LOG: RTT: Failed to find RTT control block in vector table, trying to find it in RAM.
LOG: RTT: Search ranges: 0x22060000 - 0x22070000 
LOG: Periodic RTT: Looking for RTT CB @ 0x22062000 (Range 1/1)
LOG: RTT: Failed to find RTT control block in RAM.
LOG: Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD
LOG: RTT: Failed to find RTT control block in vector table, trying to find it in RAM.
LOG: RTT: Search ranges: 0x22060000 - 0x22070000 
LOG: Periodic RTT: Looking for RTT CB @ 0x22063000 (Range 1/1)
LOG: RTT: Failed to find RTT control block in RAM.
LOG: Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD
LOG: RTT: Failed to find RTT control block in vector table, trying to find it in RAM.
```

- 日志分析

以上日志完整展示了 RTT Viewer 自动探测失败的过程，每一步都可以从日志中找到对应线索：

```
RTT Viewer 连接成功
  │
  ▼
步骤1：向量表方案
  │  ← LOG: "Periodic RTT: Memory read error when reading RTT CB @ 0x069C6CAD"
  │
  ├─ J-Link 读取 VTOR 寄存器 → 得到向量表基址
  ├─ 读取 (基址 + 0x20) 处的 32-bit 值
  ├─ 值 = 0x069C6CAF（bit[0]=1 → 被判定为有效 RTT 地址）
  └─ 尝试读 RTT CB @ 0x069C6CAD → 内存访问错误！
        │
        ▼
步骤2：RAM 搜索方案
  │  ← LOG: "Failed to find RTT control block in vector table, trying to find it in RAM"
  │  ← LOG: "Search ranges: 0x22060000 - 0x22070000"
  │
  ├─ 在 J-Link 数据库配置的 RAM 范围内搜索 "SEGGER RTT"
  ├─ 范围仅 64KB（0x22060000 - 0x22070000）
  └─ _SEGGER_RTT 实际在 0x2200042C，不在范围内 → 找不到！
        │
        ▼
循环：步骤1 → 步骤2 → 步骤1 → ...（约每 10ms 重复一次）
```

**日志中的关键线索：**

| 日志内容 | 含义 | 说明 |
|:---|:---|:---|
| `0x069C6CAD` | 向量表+0x20读出的RTT地址 | 该地址从 (VTOR+0x20) 计算得出，bit[0]=1 被J-Link认为是有效地址 |
| `Memory read error` | 该地址不可读 | 0x069C6CAD 指向的区域没有有效内存映射 |
| `0x22060000 - 0x22070000` | RAM搜索范围仅64KB | 芯片实际RAM为 0x22000000 - 0x221D4000（约1.9MB） |

**为什么向量表方案会拿到 0x069C6CAD？** 经实测，0x069C6CAF 来自地址 0x00000020 的遗留数据（该地址会别名映射到 Flash 的某个区域）。VTOR 寄存器指向的向量表中，偏移 0x20 位置没有存放正确的 `(_SEGGER_RTT + 2)`，而是遗留的代码数据恰好 bit[0]=1，被误判为 RTT 地址。

SEGGER 官方推荐在中断向量表偏移 0x20 处存储 RTT 控制块地址：

```c
// 示例向量表
_vectors:
        VECTOR __stack_end__          // 0x00: 初始栈指针
        VECTOR Reset_Handler          // 0x04: Reset
        ISR_HANDLER NMI_Handler       // 0x08: NMI
        VECTOR HardFault_Handler      // 0x0C: HardFault
        ISR_HANDLER MemManage_Handler // 0x10: MemManage
        ISR_HANDLER BusFault_Handler  // 0x14: BusFault
        ISR_HANDLER UsageFault_Handler// 0x18: UsageFault
        ISR_RESERVED                  // 0x1C: Reserved
        .word (_SEGGER_RTT + 2)       // 0x20: RTT 控制块地址！
        ...
```

- 为什么需要 +2

```
_SEGGER_RTT 实际地址 = 0x20000848  (偶数，bit[0]=0)

向量表中存储 = 0x20000848 + 2 = 0x2000084A  (bit[0]=1)

J-Link 读取后判断:
    if (地址 & 1) {
        // bit[0]=1，这是 RTT 控制块地址
        实际地址 = 地址 - 2;
    } else {
        // bit[0]=0，这是普通的中断向量
    }
```

- ARM Thumb 指令集要求函数地址 bit[0]=1
- +2 确保 bit[0]=1，让 J-Link 区分 RTT 地址和普通中断向量

# 五、自动探测原理深入分析

## 5.1 自动探测的三个模式

根据SEGGER官方文档，RTT控制块检测有三种模式：

- 模式1：向量表地址检测（优先）
    - J-Link首先检查向量表偏移0x20处
    - 如果有有效地址（bit[0]=1），直接使用
    - 这是最快、最可靠的方式

- 模式2：RAM区域自动搜索（默认）
    - 如果向量表中没有地址，J-Link会在设备数据库配置的RAM范围内搜索
    - 搜索"SEGGER RTT"字符串标识
    - **关键发现**：搜索范围是J-Link设备数据库中配置的RAM地址，不是整个内存空间

- 模式3：手动指定地址
    - 用户通过J-Link命令字符串手动指定RTT控制块地址
    - 使用`SetRTTAddr`命令
    - 或通过`SetRTTSearchRanges`指定搜索范围

## 5.2 自动探测失败的根本原因

通过测试发现，自动探测失败的根本原因是：

1. **RTT控制块不在J-Link设备数据库的RAM范围内**
   - J-Link设备数据库为每个芯片配置了特定的RAM地址范围
   - 如果RTT控制块放在数据库范围之外，自动搜索就会失败

2. **验证方法**
   - 读取DLL设备信息，获取flash和RAM地址范围
   - 修改MCU RTT控制块到该RAM地址上
   - 验证自动探测是否成功

3. **测试结论**
   - 自动探测的原理就是搜索SEGGER数据库的RAM地址
   - 如果RTT控制块不在这个范围内，就必须手动指定

## 5.3 四个工程测试验证

- 测试工程1：RA6M5 (Cortex-M33 @ 200MHz)
    - **内存架构**：Flash + 单SRAM
    - **链接文件**：fsp_rtt.ld
    - **测试方法**：修改链接文件，将RTT控制块放到J-Link数据库的RAM范围内
    - **结果**：自动探测成功

- 测试工程2：RA8D1 (Cortex-M85)
    - **内存架构**：MRAM + TCM + SRAM
    - **链接文件**：fsp_rtt.ld
    - **测试方法**：调整RTT控制块位置到数据库配置的RAM区域
    - **结果**：自动探测成功

- 测试工程3：RZN2L (Cortex-R52)
    - **内存架构**：TCM + SRAM
    - **链接文件**：fsp_xspi0_boot_rtt.ld
    - **测试方法**：将RTT控制块放在非缓存内存区域
    - **结果**：自动探测成功

- 测试工程4：RA8P1 (Cortex-M85)
    - **内存架构**：SRAM0/1/2 (0x22000000) + TCM
    - **链接文件**：fsp.lld (IAR)
    - **测试方法**：分析别名地址问题，验证 RAM 搜索范围影响
    - **结果**：见 6.4 节详细分析

# 六、问题分析

## 6.1 瑞萨 FSP 向量表定义

查看 `ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.c`：

```c
BSP_DONT_REMOVE const exc_ptr_t __VECTOR_TABLE[BSP_CORTEX_VECTOR_TABLE_ENTRIES] = {
    (exc_ptr_t) (&g_main_stack[0] + BSP_CFG_STACK_MAIN_BYTES), /* 0x00: Initial Stack Pointer */
    Reset_Handler,                                             /* 0x04: Reset Handler         */
    NMI_Handler,                                               /* 0x08: NMI Handler           */
    HardFault_Handler,                                         /* 0x0C: Hard Fault Handler    */
    MemManage_Handler,                                         /* 0x10: MPU Fault Handler     */
    BusFault_Handler,                                          /* 0x14: Bus Fault Handler     */
    UsageFault_Handler,                                        /* 0x18: Usage Fault Handler   */
    SecureFault_Handler,                                       /* 0x1C: Secure Fault Handler  */
    0,                                                         /* 0x20: Reserved ← 空！      */
    0,                                                         /* 0x24: Reserved              */
    0,                                                         /* 0x28: Reserved              */
    SVC_Handler,                                               /* 0x2C: SVCall Handler        */
    DebugMon_Handler,                                          /* 0x30: Debug Monitor Handler */
    0,                                                         /* 0x34: Reserved              */
    PendSV_Handler,                                            /* 0x38: PendSV Handler        */
    SysTick_Handler,                                           /* 0x3C: SysTick Handler       */
};
```

**问题：偏移 0x20 处是 `0`（Reserved），没有 RTT 控制块地址！**


## 6.2 瑞萨 FSP 内存布局

从 map 文件可以看到：

```
RAM 起始: 0x20000000

__ram_noinit$$  0x20000000      0x41c   ← 堆、栈（瑞萨设计：放在开头）
__ram_zero$$    0x2000041c      0x530   ← BSS 段
    ├── _acDownBuffer
    ├── _acUpBuffer
    └── _SEGGER_RTT  0x20000848         ← RTT 控制块在 ~2KB 位置
.ram.endof      0x20000c00
```

- 瑞萨设计与传统 .S 启动文件布局对比

| 项目 | 瑞萨 FSP 布局 | 传统 .S 启动文件布局 |
| :--- | :--- | :--- |
| RAM 开头 | 堆、栈 | BSS 段（全局变量） |
| BSS 位置 | 0x2000041c 之后 | 0x20000000 附近 |
| RTT 控制块 | ~2KB 位置 | 通常在前 1KB |

瑞萨将堆栈放在 RAM 开头的好处：栈从 RAM 起始地址向下增长，不会与 BSS 段的全局变量冲突，即使栈溢出也不会第一时间破坏已初始化的变量数据。

## 6.3 搜索失败原因

1. **向量表偏移 0x20 处没有 RTT 地址**（主要原因）：FSP 生成的 `startup.c` 中该位置为 `0`（Reserved），没有放置 `(_SEGGER_RTT + 2)`
2. **J-Link 搜索 RAM 范围有限**：RTT 控制块地址不在数据库配置的 RAM 范围内，RAM 搜索无法找到
3. **RTT 控制块地址相对靠后**：堆栈放在 RAM 开头导致 BSS 段（含 `_SEGGER_RTT`）从 RAM 较高地址开始

## 6.4 RA8P1 别名地址问题

RA8P1 (Cortex-M85) 的日志中出现了 `0x069C6CAD` 这个异常地址。经实测验证，该地址来自地址 `0x00000020` 的遗留数据。

- 实测结果

```
地址 0x00000000 - 0x0000005F 的内容（与 0x10000000 区域完全相同）：
00000000 = DE9A6C6D D65C5260 8C4EBCD8 ADA79754
00000010 = A165DF9C 97D28907 733E8A8E 275B0E9B
00000020 = 069C6CAF 75888416 BF12F0FF 4838D167  ← offset 0x20 的值
```

`0x00000000` 是芯片的地址别名区域，映射到 Flash 的某个物理地址。当 VTOR 指向 `0x00000000` 时，CPU 会从该别名区域读取向量表。而 offset 0x20 处恰好有一段遗留代码数据 `0x069C6CAF`，其 bit[0]=1，被 J-Link 误判为有效的 RTT 控制块地址。

- RAM 搜索范围问题

即使向量表方案失败，J-Link 还会尝试在 RAM 中搜索 RTT 控制块。查询设备数据库，RA8P1 的 RAM 搜索范围仅为：

```
搜索范围: 0x22060000 - 0x22070000（仅 64KB）
实际 RAM: 0x22000000 - 0x221D4000（约 1.9MB）
```

而 `_SEGGER_RTT` 位于 `0x2200042C`，远低于搜索范围起始地址 `0x22060000`，RAM 搜索必然会失败。

两个问题叠加，导致 RA8P1 的 RTT 自动探测无法成功。

# 七、解决方案

## 7.1 方案一：修改向量表

在 `startup.c` 的向量表 0x20 位置添加 RTT 控制块地址：

**要求 VTOR 指向的向量表能正确读写，且偏移 0x20 处未被用作其他用途**

- 内存布局示意

```
┌─────────────────────────────────────────────────────────────────┐
│                           Flash                                 │
├─────────────────────────────────────────────────────────────────┤
│ 0x20: 0x2000084A  ──────────────┐                              │
│       (RTT CB 地址 + 2)         │                              │
└─────────────────────────────────┼──────────────────────────────┘
                                  │
                                  │ J-Link 读取
                                  ▼
┌─────────────────────────────────────────────────────────────────┐
│                           RAM                                   │
├─────────────────────────────────────────────────────────────────┤
│ 0x20000848: ┌─────────────────────────────┐                    │
│             │ "SEGGER RTT"                │                    │
│             │ 缓冲区描述符...             │                    │
│             └─────────────────────────────┘                    │
└─────────────────────────────────────────────────────────────────┘
```

## 7.2 方案二：手动指定地址

1. 编译项目
2. 查看 map 文件中 `_SEGGER_RTT` 的地址
3. 在 J-Link RTT Viewer 中选择 "Manual" 模式
4. 输入地址（如 0x20000848）

## 7.3 方案三：使用 J-Link 命令

创建 J-Link 脚本文件 `RTT_Search.jlink`：

```
SetRTTAddr 0x20000848
```

或在 J-Link Commander 中执行：

```bash
JLinkExe -device <你的芯片型号> -if SWD -speed 4000 -CommandFile RTT_Search.jlink
```

## 7.4 方案四：修改RTT控制块到J-Link数据库RAM范围（推荐1）

- 原理
通过读取J-Link DLL设备信息，获取芯片的RAM地址范围，然后修改MCU RTT控制块到该RAM地址上，使自动探测能够成功。

- 实现步骤
1. 读取J-Link DLL设备信息，获取芯片的RAM地址范围
2. 修改链接文件，将RTT控制块放到数据库配置的RAM区域内
3. 重新编译项目
4. 验证自动探测是否成功

- 优势
- 无需手动输入地址
- 无需修改向量表
- 适用于各种芯片架构

## 7.5 方案五：RTT软件检索map文件（反向自动探测）（推荐2）

- 原理
RTT软件自动检索map文件，找到`_SEGGER_RTT`对应的地址，填入手动地址配置，实现"反向自动探测"。

- 实现步骤
1. 编译项目生成map文件
2. RTT软件解析map文件，找到`_SEGGER_RTT`地址
3. 自动填入手动地址配置
4. 无需用户手动查找地址

## 7.6 方案六：使用非缓存SRAM地址

- 原理
对于有缓存的芯片，将RTT控制块放在非缓存的SRAM地址上，确保J-Link能直接访问。

- 实现方法
参考RZN2L项目，将RTT控制块放在非缓存内存区域：

```c
// SEGGER_RTT.c
SEGGER_RTT_CB _SEGGER_RTT __attribute__ ((section (".noncache_buffer._SEGGER_RTT")));
```

```ld
// 链接脚本
.noncache_buffer NONCACHE_BUFFER_START (NOLOAD)
{
    KEEP(*(.noncache_buffer._SEGGER_RTT))
    KEEP(*(.noncache_buffer._acUpBuffer))
    KEEP(*(.noncache_buffer._acDownBuffer))
} > NONCACHE_BUFFER
```

## 7.7 方案七：固定MCU控制块地址（推荐3）

- 原理
在链接文件中固定RTT控制块的地址，然后在J-Link中手动输入固定地址。

- 实现步骤
1. 在链接文件中固定RTT控制块地址
2. 在J-Link RTT Viewer中手动输入固定地址
3. 每次编译后地址不变，无需重新输入

# 八、配置参数说明

## 8.1 缓冲区配置

| 宏定义 | 默认值 | 说明 |
| :--- | :--- | :--- |
| BUFFER_SIZE_UP | 1024 | 上行缓冲区大小（目标→主机） |
| BUFFER_SIZE_DOWN | 16 | 下行缓冲区大小（主机→目标） |
| SEGGER_RTT_MAX_NUM_UP_BUFFERS | 3 | 最大上行通道数 |
| SEGGER_RTT_MAX_NUM_DOWN_BUFFERS | 3 | 最大下行通道数 |

## 8.2 缓冲区模式

| 模式 | 说明 |
| :--- | :--- |
| SEGGER_RTT_MODE_NO_BLOCK_SKIP | 非阻塞跳过：缓冲区满时丢弃全部数据 |
| SEGGER_RTT_MODE_NO_BLOCK_TRIM | 非阻塞截断：缓冲区满时写入能容纳的部分 |
| SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL | 阻塞：缓冲区满时等待直到有空间 |

## 8.3 应用层接口

```c
// common_utils.h
#define APP_PRINT(fn_, ...)      SEGGER_RTT_printf(0, (fn_), ##__VA_ARGS__)
#define APP_ERR_PRINT(fn_, ...)  SEGGER_RTT_printf(0, "[ERR]...", ...)
#define APP_ERR_TRAP(err)        if(err) { SEGGER_RTT_printf(...); __asm("BKPT #0"); }
```

# 九、Cortex-R52 对比（RZN2L）

RA 系列使用 Cortex-M 内核，RZN2L 使用 **Cortex-R52**，两者在 RTT 实现上有本质区别。

## 9.1 向量表格式对比

| 架构 | 向量表内容 | 0x20 方案 |
| :--- | :--- | :--- |
| Cortex-M | 存储**地址**，CPU 自动跳转 | ✅ 可用 |
| Cortex-R | 存储 **`LDR PC` 指令**，手动加载地址 | ❌ 不可用 |

```c
// Cortex-M 向量表（FSP startup.c）
const exc_ptr_t __VECTOR_TABLE[] = {
    (exc_ptr_t)(&_stack_end),      // 0x00: 地址
    Reset_Handler,                  // 0x04: 地址
    ...
    0,                              // 0x20: 可放 RTT 地址
};

// Cortex-R 向量表（RZN2L）
__asm volatile (
    "    LDR pc,=Reset_Handler      \n"  // 0x00: 指令
    "    LDR pc,=Undefined_Handler  \n"  // 0x04: 指令
    ...
    "    LDR pc,=FIQ_Handler        \n"  // 0x1C: 指令
    // ← 没有多余空间！
);
```

## 9.2 RZN2L 内存布局

```
0x00000000 ┌─────────────────────┐
           │  ATCM (128KB)       │ ← 代码运行
0x0001FFFF ├─────────────────────┤
0x00100000 │  BTCM (128KB)       │ ← Loader + 各种栈
0x0011FFFF ├─────────────────────┤
0x10000000 │  SYSTEM_RAM (1.5MB) │
0x1017FFFF ├─────────────────────┤
0x30000000 │  SYSTEM_RAM_MIRROR  │ ← 非缓存访问
0x30160004 │    └── _SEGGER_RTT  │ ← RTT 控制块
0x3017FFFF └─────────────────────┘
```

## 9.3 RZN2L 的 RTT 方案

将 RTT 控制块放在**非缓存内存区域**，确保 J-Link 能直接访问：

```c
// SEGGER_RTT.c
SEGGER_RTT_CB _SEGGER_RTT __attribute__ ((section (".noncache_buffer._SEGGER_RTT")));
```

```ld
// 链接脚本
.noncache_buffer NONCACHE_BUFFER_START (NOLOAD)
{
    KEEP(*(.noncache_buffer._SEGGER_RTT))
    KEEP(*(.noncache_buffer._acUpBuffer))
    KEEP(*(.noncache_buffer._acDownBuffer))
} > NONCACHE_BUFFER
```

## 9.4 自动搜索方案对比

| 启动文件类型 | 内核 | 向量表 0x20 | RTT 位置 | 自动搜索现状 |
| :--- | :--- | :--- | :--- | :--- |
| .S 启动文件（传统） | Cortex-M | ✅ 通常保留为 Reserved | SRAM 开头 | ✅ 常见成功 |
| FSP startup.c（RA6M4） | Cortex-M33 | ❌ 初始化为 0 | SRAM ~2KB | ❌ 向量表+RAM均失败 |
| FSP startup.c（RA8P1） | Cortex-M85 | ❌ 别名地址中为遗留数据 | 0x2200042C | ❌ 向量表+RAM均失败 |
| Cortex-R 启动（RZN2L） | Cortex-R52 | ❌ 无此机制 | 非缓存区域 | ❌ 需手动指定 |

# 十、总结

- 关键发现

1. **根本原因**：自动探测失败是因为RTT控制块不在J-Link数据库的RAM范围内
2. **验证方法**：通过读取DLL设备信息的flash RAM范围，修改MCU RTT控制块到该RAM地址上，验证自动探测是否成功
3. **核心原理**：自动探测的原理就是搜索SEGGER数据库的RAM地址
4. **别名地址问题**：部分芯片存在地址别名映射，VTOR 指向的地址可能不包含有效的向量表数据
5. **RAM 搜索范围过窄**：RA8P1 的 RAM 搜索范围仅 64KB (0x22060000-0x22070000)，实际 RAM 达 1.9MB
6. **测试验证**：测试了4个代表性芯片工程（RA6M5、RA8D1、RA8P1、RZN2L），验证解决方案的有效性

- 各架构最佳方案

| 启动文件类型 | 推荐方案 | 说明 |
| :--- | :--- | :--- |
| .S 启动文件（传统） | 通常无需修改 | 向量表 0x20 通常保留为 Reserved，部分 IDE 已自动添加 RTT 地址 |
| FSP startup.c（瑞萨 RA） | 修改RTT控制块到数据库RAM范围 | 调整链接文件，推荐方案1 |
| Cortex-R（RZN2L） | 使用非缓存SRAM地址 | 将RTT控制块放在非缓存内存区域 |



- 解决方案对比

| 方案 | 优点 | 缺点 | 适用场景 |
| :--- | :--- | :--- | :--- |
| 向量表0x20方案 | 官方推荐 | 需要修改启动文件，VTOR路径可能无效 | Cortex-M（传统.S启动文件） |
| **修改到数据库RAM范围（推荐1）** | 自动探测成功，通用性强 | 需要修改链接文件 | 所有芯片 |
| **RTT软件检索map文件（推荐2）** | 反向自动探测 | 需要软件支持 | 所有芯片 |
| **固定控制块地址（推荐3）** | 地址不变 | 每次需要手动输入 | 开发阶段 |
| 使用非缓存SRAM | 避免缓存问题 | 需要了解内存架构 | 有缓存的芯片 |

---
- **嵌入式科普(52)Segger-RTT-Assistant v1.4.2，完美支持修改到数据库RAM范围（推荐1）、RTT软件检索map文件（推荐2），介绍上面自动搜索可能失败的问题**
---
- 参考链接

    - [SEGGER RTT 官方文档](https://kb.segger.com/RTT)
    - [SEGGER RTT GitHub](https://github.com/SEGGERMicro/RTT)
    - [J-Link Command Strings](https://kb.segger.com/J-Link_Command_Strings)
