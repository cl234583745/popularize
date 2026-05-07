嵌入式科普(18)Ubuntu在移动硬盘的安装和启动
===
[toc]
# 一、概述
- 在移动硬盘安装Ubuntu
- 从电脑BIOS启动移动硬盘Ubuntu
- 从VMware启动移动硬盘Ubuntu


# 二、应用场景
- 家里使用台式机
- 上班或外出使用笔记本
- 并且笔记本硬盘小
- 在多台电脑使用1个Ubuntu


```mermaid
graph LR;
    S1(场景1)-->A(家里<br>台式机) -->|同时| C(上班或出差<br>笔记本) -->|并且硬盘小|B(矛盾：<br>既要也要);
    S2(场景2)-->D(需要多台电脑使用1个Ubuntu) ---> B;
    
  ```
# 三、移动硬盘安装Ubuntu
方法有多种，这里介绍使用VMware，在移动硬盘安装Ubuntu
参考资料：https://blog.csdn.net/qq_23090647/article/details/122517349

## 3.1 移动硬盘格式化
- **不要使用系统自带的，极慢。使用DiskGenius**

## 3.2 VMware安装Ubuntu到移动硬盘
- VMware试用Ubuntu
- 在试用Ubuntu中挂载移动硬盘
- 在试用Ubuntu中对移动硬盘分区
- 【重要】ESP(0)分区、分区(1)：Linux swap、分区(2)：EXT4，用于Linux系统的 “/” 目录
- 在试用Ubuntu中安装Ubuntu到移动硬盘的/dev/sda1
- 到此搞定
- 如果移动硬盘较大，可以Ubuntu 150G，其余window硬盘

# 四、电脑BIOS启动移动硬盘Ubuntu
- 进入BIOS设置移动硬盘为最优先启动项
- 以上就是：单独启动Ubuntu或单独启动window
  
# 五、从VMware启动移动硬盘Ubuntu
- window中运行VMware，使用VMware引导硬盘Ubuntu
- window和Ubuntu同时运行，可能方便一点

参考资料：https://blog.csdn.net/u014291571/article/details/136473305?spm=1001.2014.3001.5502

# 六、问题解决(坑)和思考提问
- 台式机：BIOS启动Ubuntu正常；VMware启动Ubuntu也正常
- 笔记本：BIOS启动Ubuntu正常；**VMware启动Ubuntu异常**
- 提示错误：piixx4_smbus 0000:00:07:.3: SMBus base address uninitialized - upgrade BIOS or use force_addr=0xaddr.
- 并且过一段时间后电脑的win10右下角就是体会无法识别移动硬盘，然后不论win10还是VMware都无法再次识别了，即使重新插拔硬盘，只能重启电脑了
- **坑：台式机用USB typeA接移动硬盘，笔记本用typeC。笔记本也是typeA则正常（typeC转typeA）**
- 提问：是因为在台式机制作“移动硬盘安装Ubuntu”时候使用的是typeA？所以笔记本VMware也需要使用typeA？或者VMware引导移动硬盘Ubuntu就是不支持typeC呢？