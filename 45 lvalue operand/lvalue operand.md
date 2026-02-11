嵌入式科普(45)C语言取地址运算符&的秘密
===

[toc]

# 一、概述
- 实际代码碰到的小问题，总结相关知识点


# 二、编译阶段固定变量到Flash
```
typedef struct {
    uint32_t * src;
    uint32_t * dst;
    uint32_t size;
    uint32_t enable_flag;
} loader_table;

defined(__GNUC__)// .ld文件变量
extern uint32_t APP_FLASH_section_start;
extern uint32_t APP_RAM_start;
extern uint32_t APP_FLASH_section_size;

#define offset          0x0000005c
#define prg_flash_addr (&APP_FLASH_section_start)
#define prg_start_addr (&APP_RAM_start)
#define prg_size       (&APP_FLASH_section_size)
#endif

//固定Flash位置 const变量
const loader_table table[TABLE_ENTRY_NUM] BSP_PLACE_IN_SECTION("CPU0_LOADER_TABLE") =
{
  { (uint32_t *)prg_flash_addr, (uint32_t *)prg_start_addr, (uint32_t)prg_size, (uint32_t)TABLE_ENABLE },
};
```

## 2.1 变更需求：APP_FLASH_section_start偏移0x5c
```
        原始代码： #define prg_flash_addr  (&APP_FLASH_section_start)
```

### 2.1.1 错误修改1
#define prg_flash_addr  (&(APP_FLASH_section_start + offset))
```
    编译错误信息是："lvalue required as unary '&' operand"。

    C11标准 (6.5.3.2 Address and indirection operators):
    "The operand of the unary & operator shall be either a function designator or an lvalue that designates an object that is not a bit-field and is not declared with the register storage-class specifier."
    一元&运算符的操作数必须是函数指示符或左值，且该左值不能是位域，也不能用register存储类说明符声明。
```
**核心规则**：
&只能作用于左值(lvalue)
左值：有明确存储位置的对象，可以出现在赋值号左边
右值：没有明确存储位置的临时值，不能出现在赋值号左边

```
// C/C++通用示例
int arr[10];
int x = 10;

// 正确：左值
int *p1 = &x;           // x是左值
int *p2 = &arr[0];      // arr[0]是左值
int *p3 = &(*p1);       // *p1是左值

// 错误：右值
// int *p4 = &10;       // 10是右值
// int *p5 = &(x + 1);  // x+1是右值
// int *p6 = &(x++);    // x++是右值（返回原始值的副本）
int *p7 = &(++x);       // 正确：++x是左值（返回x本身）

// 函数相关
void func();
// void (*fp)() = &func; // 正确：函数指示符
// void (*fp)() = &(func); // 正确：函数名是左值
```

|运算符|官方名称|操作数要求|返回值类型|备注|
|:-:|:-:|:-:|:-:|:-:|
|&|取地址运算符|左值（或函数）|指针类型|不能用于右值、位域、register变量|
|*|解引用运算符|指针类型|左值引用|可以用于赋值号左边|

### 2.1.2 错误修改2
#define prg_flash_addr  (&APP_FLASH_section_start + offset)
```
    编译正常，运行正常，但运行结果不对！
    调试发现prg_flash_addr = 0x60100170？应该为0x6010005c
    0x5c x 4 = 0x170
```
**关键：指针算术的原理**
在C语言中，当给指针加上一个整数N时，实际的偏移量是N * sizeof(指针指向的类型)。

### 2.1.3 正确修改1
#define prg_flash_addr  ((uint32_t)&APP_FLASH_section_start + offset)
### 2.1.4 正确修改2
#define prg_flash_addr ((uint32_t *)((uint8_t *)&APP_FLASH_section_start + offset))

# 三、总结
- C语言取地址运算符&只能修饰"左值"
- C语言指针算术偏移量是N * sizeof
- 灵活运用指针的重要性