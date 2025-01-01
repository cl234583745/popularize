嵌入式科普(26)为什么heap通常8字节对齐
===
[toc]
# 一、概述
- 结论：在嵌入式c语言中，heap通常8字节对齐
- 本文主要分析这个问题的分析过程

# 二、newlibc heap
newlibc:
https://github.com/blueskycoco/newlibc/tree/master/newlib/libc

![](./images/newlibc%20align.png)

## 2.1 stm32cubeide .ld heap
stm32 .ld
```
  /* User_heap_stack section, used to check that there is enough "RAM" Ram type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM
```

## 2.2 e2studio .ld heap
ra6m3
```
    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM
```




```
/* real_size is the size we actually have to allocate, allowing for
   overhead and alignment.  */
#define REAL_SIZE(sz)						\
  ((sz) < sizeof (struct freelist_entry) - sizeof (size_t)	\
   ? sizeof (struct freelist_entry)				\
   : sz + sizeof (size_t) + M_ALIGN(sz, sizeof (size_t)))
```

# 三、glibc源码
glibc:
https://github.com/bminor/glibc/tree/master


![](./images/glibc%20align.png)
```
/* MALLOC_ALIGNMENT is the minimum alignment for malloc'ed chunks.  It
   must be a power of two at least 2 * SIZE_SZ, even on machines for
   which smaller alignments would suffice. It may be defined as larger
   than this though. Note however that code and data structures are
   optimized for the case of 8-byte alignment.  */
#define MALLOC_ALIGNMENT (2 * SIZE_SZ < __alignof__ (long double) \
			  ? __alignof__ (long double) : 2 * SIZE_SZ)

```
## 3.1 Ubuntu c heap
Ubuntu：
![](./images/Ubuntu%20align%20check.png)
```
#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h> 

int main() {  
    // 不同大小的请求，包括不是8的倍数  
    size_t sizes[] = {15, 16, 23, 24, 31, 32, 64, 127, 128, 255, 256, 1023, 1024};   
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);  

    for (size_t i = 0; i < num_sizes; i++) {  
        void *ptr = malloc(sizes[i]);  

        // 检查 malloc 是否成功  
        if (ptr == NULL) {  
            perror("malloc failed");  
            return EXIT_FAILURE;  
        }  

        // 获取指针的地址  
        uintptr_t addr = (uintptr_t)ptr;  

        // 检查地址是否是8字节对齐  
        if (addr % 8 == 0) {  
            printf("Pointer for size %zu is 8-byte aligned: %p\n", sizes[i], ptr);  
        } else {  
            printf("Pointer for size %zu is NOT 8-byte aligned: %p\n", sizes[i], ptr);  
        }  

        // 释放分配的内存  
        free(ptr);  
    }  

    return EXIT_SUCCESS;  
} 
```
```
jerry@jerry-virtual-machine:~/桌面$ ./check
Pointer for size 15 is 8-byte aligned: 0x5615aab2d2a0
Pointer for size 16 is 8-byte aligned: 0x5615aab2d2a0
Pointer for size 23 is 8-byte aligned: 0x5615aab2d2a0
Pointer for size 24 is 8-byte aligned: 0x5615aab2d2a0
Pointer for size 31 is 8-byte aligned: 0x5615aab2d6d0
Pointer for size 32 is 8-byte aligned: 0x5615aab2d6d0
Pointer for size 64 is 8-byte aligned: 0x5615aab2d700
Pointer for size 127 is 8-byte aligned: 0x5615aab2d750
Pointer for size 128 is 8-byte aligned: 0x5615aab2d750
Pointer for size 255 is 8-byte aligned: 0x5615aab2d7e0
Pointer for size 256 is 8-byte aligned: 0x5615aab2d7e0
Pointer for size 1023 is 8-byte aligned: 0x5615aab2d8f0
Pointer for size 1024 is 8-byte aligned: 0x5615aab2d8f0
```

```
jerry@jerry-virtual-machine:~/桌面$ uname -m
x86_64

cat /usr/lib/x86_64-linux-gnu/ldscripts/elf32_x86_64.x 
```

# 四、总结

- 不论是在STM32的32位微控制器上还是在64位的Ubuntu系统中，heap 8字节对齐都因其带来的**性能优化、硬件兼容性和代码可移植性**而成为一种广泛接受的标准。这种做法帮助开发者构建既高效又可靠的应用程序，**符合现代计算环境的需求**。