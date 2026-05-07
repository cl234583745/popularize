嵌入式科普(36)【程序员必看】GCC编译优化：你的代码为何“神秘消失”？
===
[toc]

# 一、概述
- 明明代码逻辑无误，但是debug就是不运行，似乎代码直接“消失”了！
- 调试时一切正常，开启-O2优化后程序崩溃？
- GCC编译器优化等级到底有什么影响？

# 二、参考资料
- [GCC Optimization Options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
- [GCC Debugging Options](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html)
- [GCC and Undefined Behavior](https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Optimize-Options.html#index-undefined-behavior)
- [GCC Online Documentation](https://gcc.gnu.org/onlinedocs/)

# 三、GCC优化功能
## 3.1 GCC优化选项
- -O0：关闭优化（默认选项）。

- -O1、-O2、-O3：不同级别的优化。

- -Os：优化代码大小。

- -Ofast：激进优化，可能违反严格的标准合规性。

## 3.2 GCC优化技术

- 死代码消除（Dead Code Elimination）

- 常量传播（Constant Propagation）

- 函数内联（Function Inlining）

- 循环优化（Loop Optimization）

## 3.3 GCC未使用代码和变量

- 未使用的变量和函数可能会被优化掉，尤其是在启用优化选项时。

- 使用-Wall和-Wunused选项可以在编译时警告未使用的变量和函数。

## 3.4 GCC调试优化代码

- -g：生成调试信息。

- -fno-inline：禁用函数内联。

- -fno-eliminate-unused-debug-types：保留未使用的调试信息。

## 3.5 GCC优化对代码行为的影响


- 未初始化的变量。

- 越界访问。

- 违反严格的别名规则。

# 四、代码“消失”的场景


- 死代码消除
```
int x = 10;
x = 20;  // 第一次赋值被优化掉
printf("%d", x);  
```


- 未使用的变量：没用过的东西，留着占地方？
```
int y = 30;  
printf("Hello");  
```

- 永远不执行的循环
```
for (int i=0; i<0; i++) {  
    printf("这循环能执行算我输！");  
}  
```

- 常量条件判断
```
const int debug = 0;  
if (debug) {  
    printf("这条日志你永远看不到！");  
}  
```


- 冗余赋值
```
int a = 10;  
a = 20;  // 前一条赋值被优化  
printf("%d", a);  
```

- 编译器版本和死代码
  gcc新版本12、13等优化更严格
```
volatile unsigned int coremarkTick;
void g_cmtw0CB(timer_callback_args_t *p_args)
{
    //oid)p_args;
    coremarkTick++;
    
}
```

```
printf("==============");
printf("start\n");

printf("end\n");
printf("==============");
```
# 五、防止代码“被优化”
## 5.1 关闭优化：使用-O0选项

```
gcc -O0 -o program program.c
```


## 5.2 volatile关键字

```
volatile int sensor_value; // 禁止优化此变量
```

## 5.3 启用编译警告：使用-Wall
```
gcc -Wall -O2 -o program program.c
```

## 5.4 单独优化等级

```
#pragma GCC optimize ("O1")
#pragma GCC reset_options
```

## 5.5 写代码规范化
- 最重要

# 六、总结
- GCC优化会加速代码运行效率
- GCC最新版本对代码的规范化更严格
- 合理的设置优化等级、单独优化、防止优化等功能
- 分享你遇到的非期望优化例子
