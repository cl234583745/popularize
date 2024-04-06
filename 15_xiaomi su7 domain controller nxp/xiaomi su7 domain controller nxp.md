嵌入式科普(15)小米su7成本分析和拆解之智驶、座舱分析
===
[toc]
# 一、概述
- 综合网上信息，分享数据
- 域控分析
- NXP S32K324 汽车通用微控制器一览
![](./images/su7.png)  
# 二、小米su7成本分析
- [【zxqcq-8757】小米SU7单车成本拆解.docx](./doc/【zxqcq-8757】小米SU7单车成本拆解.docx)
## 2.1 整车成本构成
![](./images/list1.png)
![](./images/percent1.png)

## 2.2 三电系统
![](./images/list2.png)
![](./images/percent2.png)

## 2.3 车身与底盘
![](./images/list3.png)
![](./images/percent3.png)

## 2.3 智能网联
![](./images/list4.png)
![](./images/percent4.png)

## 2.4 内外饰
![](./images/list5.png)
![](./images/percent5.png)

来源：电驱动Benchmarker
注：本文版权归原作者所有，文中观点仅供分享交流，不代表本公众号观点和立场。如涉及版权等问题，请您告知，我们将及时处理。

# 三、小米su7拆解之智驶、座舱分析
**拆解图片---抖音：杨长顺维修家**
## 3.1 主要芯片
- Qualcomm SA8295P
- Qualcomm PMM8295AU
- NVIDIA 2347A1
- Qualcomm QCA6595  
- Qualcomm QCA6696  
- NXP S32K324

## 3.2 智能驾驶&智能座舱 

![](./images/1.png)
![](./images/2.png)
![](./images/3.png)
![](./images/4.png)
![](./images/5.png)
![](./images/6.png)
![](./images/7.png)
![](./images/8.png)
![](./images/9.png)
![](./images/10.png)
![](./images/11.png)
![](./images/12.png)
![](./images/13.png)
![](./images/14.png)
![](./images/15.png)
![](./images/16.png)
![](./images/17.png)
![](./images/18.png)
![](./images/19.png)
![](./images/20.png)
![](./images/21.png)
![](./images/22.png)
![](./images/23.png)
![](./images/24.png)
![](./images/25.png)

# 四、NXP S32K324汽车通用微控制器
- [NXP-S32K3xx datasheet.pdf](./doc/NXP-S32K3xx.pdf)
- [S32K3xx Reference Manual.pdf](./doc/S32K3XXRM.pdf)
- [NXP: https://www.nxp.com.cn/products/processors-and-microcontrollers/s32-automotive-platform/s32k-auto-general-purpose-mcus/s32k3-microcontrollers-for-automotive-general-purpose:S32K3](https://www.nxp.com.cn/products/processors-and-microcontrollers/s32-automotive-platform/s32k-auto-general-purpose-mcus/s32k3-microcontrollers-for-automotive-general-purpose:S32K3)

S32K3系列32位微控制器(MCU)提供基于Arm® Cortex®-M7的MCU，支持单核、双核和锁步内核配置。S32K3系列具有内核、内存和外设数量方面的可扩展性，能够实现高性能和功能安全，符合ISO26262标准，达到ASIL D安全等级。

S32K3系列提供全面的端到端解决方案，涵盖从开发到生产的各个环节。S32K3 MCU具有带恩智浦固件的硬件安全引擎(HSE)，支持无线固件更新(FOTA)，并为AUTOSAR® 和非AUTOSAR应用免费提供符合ISO26262的实时驱动(RTD)。

S32K3系列与恩智浦S32汽车平台兼容，实现了无缝软件重复使用和灵活性，适用于车身、区域控制和电气化应用。

S32K31 MCU提供多种封装类型，包括MAPBGA、LQFP和HDQFP，在集成和设计方面提供了灵活性。此外，与标准QFP封装相比，恩智浦HDQFP封装的尺寸减少了55%。

![](./images/s32k324.png)
![](./images/s32k%20roadmap.png)
![](./images/s32k%20list.png)
![](./images/s32k%20block.png)
![](./images/s32k%20function.png)
![](./images/s32k324%20ASIL%20B.png)