嵌入式科普(17)c语言void and void*含义和应用
===
[toc]
# 一、概述
- void and void*含义和用法
- void and void*的**实际应用**
  
# 二、void类型
## 2.1 void“无类型”
void 是一个特殊的类型，表示“无类型”。它不能用于声明变量（即不能有一个类型为 void 的变量），但它经常用于以下情况：
### 2.1.1 函数返回void
如果一个函数不返回任何值，它的返回类型就是 void。
```
void printMessage() {  
    printf("Hello, World!\n");  
}
```
### 2.1.2 函数参数void
表示该函数不接受任何参数。
```
void printMessage(void) { // 注意：这里的void是可选的，但在某些上下文中可能是为了明确性而使用的  
    printf("Hello, World!\n");  
}
```

### 2.1.3 void (*)(void)
void (*)(void)：这是一个函数指针的类型，它指向一个返回类型为 void 并且没有参数（参数类型为 void）的函数。

typedef void (*fsp_vector_t)(void); 这一行代码定义了一个函数指针类型名为 fsp_vector_t。这个函数指针指向的函数没有参数（void）并且没有返回值（void）。
```
typedef void (*fsp_vector_t)(void);
```

## 2.2 断言((void)0U)

- ((void)0) 是一个没有实际作用的表达式，通常用作占位符或消除编译器警告。
- 注意断言((void)0U)不会阻塞或者产生hardfault，需另外处理(不同于Error_Handler();)
  
```
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* Check the parameters */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
  }
```
```
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */
//////////////////////////////////////////////////
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```
```
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
```

# 三、void*指针
## 3.1 “无类型”的指针
void* 是一个特殊的指针类型，表示指向“无类型”的指针。这意味着它可以指向任何类型的数据，但你不能直接对它进行解引用或算术运算（除非你将其转换为指向特定类型的指针）。
- void* 指针可以指向任意类型的数据
- void* 指针不允许进行 ++ 或 -- 操作
- void* 使用前要强制类型转换

### 3.1.1 泛型编程：
例如，在C的 qsort 函数中，比较函数接受两个 void* 参数。
- newlibc:
```
3.32 qsort—sort an array
Synopsis

#include <stdlib.h>
void qsort(void *base, size_t nmemb, size_t size,
    int (*compar)(const void *, const void *) );

Description
qsort sorts an array (beginning at base) of nmemb objects. size describes the size of each element of the array.
```

### 3.1.2 函数参数和返回值
```
void print_as_int(void* ptr) {  
    int* int_ptr = (int*)ptr;  
    printf("The integer is: %d\n", *int_ptr);  
}
```
```
// 函数分配内存并返回一个 void* 类型的指针  
void* allocate_memory(size_t size) {  
    return malloc(size); // 分配指定大小的内存，并返回 void* 类型的指针  
}  
nt main() {  
    // 分配 10 个 int 大小的内存  
    void* memory = allocate_memory(10 * sizeof(int));  
    if (memory == NULL) {  
        // 内存分配失败的处理  
        perror("Memory allocation failed");  
        return 1;  
    } 
```

## 3.2 ((void*)0)空指针
((void*)0) 用于表示一个空指针，并可以赋值给任何指针类型的变量。
#define NULL ((void*)0) 是 C 和 C++ 中 NULL 宏的传统定义之一。这个定义将 NULL 设置为一个指向 void 的空指针（null pointer）。在 C 和 C++ 中，空指针是一个特殊的指针值，表示指针不指向任何有效的内存位置。
```
#define NULL ((void*)0)
```

## 3.3 库函数使用void*
```
7.5 memccpy—copy memory regions with end-token check
Synopsis

#include <string.h>
void* memccpy(void *restrict out, const void *restrict in, 
    int endchar, size_t n);
```
```
3.24 malloc, realloc, free—manage memory
Synopsis

#include <stdlib.h>
void *malloc(size_t nbytes);
void *realloc(void *aptr, size_t nbytes);
void *reallocf(void *aptr, size_t nbytes);
void free(void *aptr);
```

## 3.4 typedef void* handle_t
- **最重要作用莫过于，驱动或库代码的“隐藏/封装”作用**
定义了一个名为 handle_t 的新类型，它是 void* 类型的别名。这种类型通常用于表示一种不透明的句柄或引用，具体的数据类型在上下文中可能是未知的或者不想直接暴露给使用者。


- r_sci_uart.c中
```
typedef void transfer_ctrl_t;

fsp_err_t R_SCI_UART_Write (uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
    sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;

    p_ctrl->tx_src_bytes = bytes - p_ctrl->data_bytes;
    p_ctrl->p_tx_src     = p_src + p_ctrl->data_bytes;
```
- r_sci_uart.h中定义sci_uart_instance_ctrl_t
```
/** UART instance control block. */
typedef struct st_sci_uart_instance_ctrl
{
    uint8_t const * p_tx_src;

    /* Size of source buffer pointer used to fill hardware FIFO from transmit ISR. */
    uint32_t tx_src_bytes;

    /* Destination buffer pointer used for receiving data. */
    uint8_t const * p_rx_dest;

    /* Size of destination buffer pointer used for receiving data. */
    uint32_t rx_dest_bytes;

    /* Pointer to the configuration block. */
    uart_cfg_t const * p_cfg;

    /* Base register for this channel */
    R_SCI0_Type * p_reg;

    void (* p_callback)(uart_callback_args_t *); // Pointer to callback that is called when a uart_event_t occurs.
    uart_callback_args_t * p_callback_memory;    // Pointer to non-secure memory that can be used to pass arguments to a callback in non-secure memory.
} sci_uart_instance_ctrl_t;
```

- 应用层代码使用api
```
void hal_entry (void)
{
    sci_uart_instance_ctrl_t g_uart0_ctrl;

    fsp_err_t err = R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) &buffer, TRANSFER_LENGTH);
```