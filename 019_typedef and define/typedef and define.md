嵌入式科普(18)typedef与#define的区别和典型应用
===
[toc]
# 一、概述
- 介绍typedef与#define的区别
- 介绍其他预处理指令指令
- typedef与#define的典型应用

# 二、typedef与#define的区别
## 2.1. 作用域
#define：

#define定义的宏没有作用域限制，一旦定义，在整个源文件中都有效（如果使用了头文件，则在所有包含该头文件的文件中都有效）。
例子：

```
#define PI 3.14159
```
在这个例子中，PI被定义为一个宏，在整个文件中都可以使用PI来代表3.14159。

typedef：

typedef定义的别名有作用域限制，通常遵循C/C++的变量和函数的作用域规则。
例子：

```
typedef int Integer;  
  
void foo() {  
    Integer i = 10; // 有效  
}  
  
int main() {  
    Integer j = 20; // 有效  
    return 0;  
}
```
在这个例子中，Integer是int的一个别名，它仅在定义它的文件或作用域中有效。

## 2.2. 类型检查
#define：

#define仅仅是文本替换，没有类型检查。如果替换后的文本不符合预期的类型，编译器在编译时可能会产生错误，但错误可能很难理解，因为问题可能来源于宏展开后的代码。
例子：

```
#define MULTIPLY(x, y) ((x) * (y))  
  
int main() {  
    int a = 5, b = 10;  
    int c = MULTIPLY(a, b); // 正确  
    int d = MULTIPLY(a, "hello"); // 错误，但错误可能不容易理解  
    return 0;  
}
```
在这个例子中，尝试将字符串与整数相乘会导致编译错误，但由于是宏替换，错误可能不容易追踪。

typedef：

typedef创建的别名是类型安全的，因为它们是基于现有类型的。
例子（与typedef本身无关，但为了展示类型安全性）：

```
typedef int Integer;  
  
void add(Integer a, Integer b) {  
    // ...  
}  
  
int main() {  
    Integer i = 10;  
    char c = 'a';  
    add(i, c); // 错误，因为c不是Integer类型  
    return 0;  
}
```
在这个例子中，尝试将char类型的变量传递给期望Integer（即int）类型的函数会导致编译错误。

## 2.3. 参数化
#define：

#define可以定义带参数的宏，实现更复杂的文本替换。
例子（如上面的MULTIPLY宏所示）。

typedef：

typedef不能定义带参数的别名。它只能为现有类型创建简单的别名。
## 2.4. 调试和可读性
#define：

由于#define只是文本替换，因此在调试时可能更难追踪和理解代码的行为。
typedef：

typedef创建的别名是类型安全的，并且在调试时更易于理解，因为它们是基于现有类型的。
## 2.5. 宏展开的时间
#define：

宏在预处理阶段展开，即在编译器开始真正的编译工作之前。
typedef：

typedef在编译阶段处理，它创建的别名是编译器可以理解的类型。

# 三、#define 等其他预处理指令
## 3.1 #include：
用于包含（或插入）其他文件的内容。
有两种形式：#include <filename>（用于标准库）和 #include "filename"（用于项目中的文件）。
## 3.2 #undef：
用于取消（或删除）先前由 #define 定义的宏。
## 3.3 条件编译：
#if, #ifdef, #ifndef, #elif, #else, #endif
这些指令允许你根据某些条件（通常是宏的定义）来选择性地编译代码块。
## 3.4 #pragma：
这是一个特定于编译器的指令，用于提供编译器特定的指令或选项。
例如，在Microsoft的编译器中，#pragma once 用于确保头文件只被包含一次。
## 3.5 #error 和 #warning：
#error 指令在预处理阶段生成一个错误消息，并停止编译。
#warning 指令生成一个警告消息，但编译过程会继续。
## 3.6 #line：
用于改变编译器报告的当前行号和文件名。
## 3.7 #（字符串化操作符）和 ##（连接操作符）：
这两个操作符在宏定义中很有用。# 操作符可以将宏参数转换为字符串，而 ## 操作符可以用于连接两个标记以形成一个新的标记。
## 3.8 预定义的宏：
编译器提供了一些预定义的宏，如 __LINE__（当前行号）、__FILE__（当前文件名）、__DATE__（编译日期）和 __TIME__（编译时间）等。
## 3.9 内联函数（Inline Functions）：
虽然不是预处理指令，但内联函数在某些方面与宏类似，因为它们都在编译时展开，以减少函数调用的开销。然而，内联函数是函数，因此它们具有作用域、类型检查和参数检查等特性。
## 3.10 constexpr（C++11 及更高版本）：
constexpr 关键字用于在编译时计算常量表达式的值。虽然它与 #define 不同，但在某些情况下，它可以作为 #define 的一个更安全、更类型安全的替代方案。
## 3.11 enum class（C++11 及更高版本）：
enum class（也称为强类型枚举或作用域枚举）提供了一种比传统 enum 更安全、更类型安全的方式来定义常量集。虽然它们不是预处理指令，但在某些情况下，它们可以作为 #define 的一个替代方案。

# 四、typedef与#define的典型应用
```
/** Blocks execution */
#define STM32_LOCK_BLOCK()                      \
  do                                            \
  {                                             \
    __disable_irq();                            \
    Error_Handler();                            \
    while (1);                                  \
  } while (0)
  ```

```
#define SQUARE(x) ((x) * (x))
```

```
#define DEBUG 1  
  
#if DEBUG  
#define PRINT_DEBUG(x) printf("Debug: %s\n", x)  
#else  
#define PRINT_DEBUG(x)  
#endif
```
```
/*******************************************************************************************************************//**
 * Enables the module stop state.
 *
 * @param      ip       fsp_ip_t enum value for the module to be stopped
 * @param      channel  The channel. Use channel 0 for modules without channels.
 **********************************************************************************************************************/
#define R_BSP_MODULE_STOP(ip, channel)             {FSP_CRITICAL_SECTION_DEFINE;                                  \
                                                    FSP_CRITICAL_SECTION_ENTER;                                   \
                                                    BSP_MSTP_REG_ ## ip(channel) |= BSP_MSTP_BIT_ ## ip(channel); \
                                                    BSP_MSTP_REG_ ## ip(channel);                                 \
                                                    FSP_CRITICAL_SECTION_EXIT;}
```
```
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE                 
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif                                           
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((__noreturn__))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#endif
```

```
typedef MyType *MyTypePtr;
```

```
typedef struct _s_pns_nv_subcfg TS_PNS_NV_SUBCFG, *TS_PNS_NV_SUBCFG_PTR;
```
