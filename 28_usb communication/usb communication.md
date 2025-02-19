嵌入式科普(24)一张图真正看懂USB通信协议
===
[toc]
# 一、概述
- USB(Universal Serial Bus)“通用”到日常生活，又复杂到工程师也不一定说的清楚
- 本文以科普的角度，用一张图并且带一定深度的把USB通信协议说尽。从宏观上或者逻辑上能理解USB


# 二、相关资料

- https://www.usb.org/document-library/usb-20-specification
- https://www.usbzh.com/
  

# 三、USB框图
![](./images/usb%20block.png)

# 四、USB相关概念的理解
## 4.1 USB 规范（USB Specification）
USB 规范是由 USB Implementers Forum (USB-IF) 制定的标准文档，定义了 USB 技术的各个方面，包括电气特性、协议、设备类、数据传输类型等。

- 主要版本
    USB 1.0/1.1：低速（1.5 Mbps）和全速（12 Mbps）。

    USB 2.0：高速（480 Mbps）。

    USB 3.0/3.1/3.2：超高速（5 Gbps、10 Gbps、20 Gbps）。

    USB4：基于 Thunderbolt 3，最高 40 Gbps。

- 举例
    USB 2.0 规范：定义了 USB 2.0 的电气特性、协议和设备类。

    USB 3.2 规范：定义了 USB 3.2 的超高速传输和双通道模式。

## 4.2 USB 接口（USB Interface）
USB 接口是 USB 设备与主机通信的逻辑通道。一个 USB 设备可以包含多个接口，每个接口对应一个功能。

- 接口描述符
    接口描述符：描述接口的类型、端点和协议。

- 接口类（Interface Class）：定义接口的功能，如 HID（人机接口设备）、CDC（通信设备类）、Mass Storage（大容量存储）等。

- 举例
    HID 接口：用于键盘、鼠标等设备。

    CDC 接口：用于串口转换器、调制解调器等设备。

    Mass Storage 接口：用于 U 盘、移动硬盘等设备。

## 4.3 USB 端点（USB Endpoint）
USB 端点是 USB 设备上的数据缓冲区，用于存储待发送或接收的数据。每个端点有一个唯一的地址和方向（IN 或 OUT）。

- 端点类型
    控制端点（Endpoint 0）：用于设备枚举和配置。

    中断端点：用于低延迟数据传输（如 HID 设备）。

    批量端点：用于大数据量传输（如 Mass Storage 设备）。

    同步端点：用于实时数据传输（如音频设备）。

- 举例
    键盘：使用中断 IN 端点发送按键数据。

    U 盘：使用批量 IN 和批量 OUT 端点传输文件数据。

## 4.4 USB 管道（USB Pipe）
USB 管道是主机和设备之间的逻辑通道，用于传输数据。每个管道对应一个端点。

- 管道类型
    控制管道：用于控制传输。

    中断管道：用于中断传输。

    批量管道：用于批量传输。

    同步管道：用于同步传输。

- 举例
    控制管道：用于设备枚举和配置。

    中断管道：用于键盘、鼠标的数据传输。

    批量管道：用于 U 盘的文件传输。

## 4.5 USB 数据传输类型
USB 定义了四种数据传输类型，每种类型对应不同的应用场景。

- 传输类型
    控制传输（Control Transfer）：用于设备枚举、配置和控制命令。

    中断传输（Interrupt Transfer）：用于低延迟数据传输（如 HID 设备）。

    批量传输（Bulk Transfer）：用于大数据量传输（如 Mass Storage 设备）。

    同步传输（Isochronous Transfer）：用于实时数据传输（如音频、视频设备）。

- 举例
    控制传输：主机发送获取设备描述符的请求。

    中断传输：键盘发送按键数据。

    批量传输：U 盘传输文件数据。

    同步传输：摄像头传输视频数据。

## 4.6 USB 设备类（USB Device Class）
USB 设备类定义了设备的功能和通信协议。常见的设备类包括：

- 常见设备类
    HID（Human Interface Device）：键盘、鼠标、游戏手柄等。

    CDC（Communications Device Class）：串口转换器、调制解调器等。

    Mass Storage：U 盘、移动硬盘等。

    Audio：耳机、麦克风等。

    Video：摄像头、视频采集卡等。

- 举例
    HID 设备类：定义了键盘、鼠标的通信协议。

    CDC 设备类：定义了串口转换器的通信协议。

    Mass Storage 设备类：定义了 U 盘的文件传输协议。

## 4.7 USB 描述符（USB Descriptors）
USB 描述符是 USB 设备向主机报告其配置和功能的数据结构。常见的描述符包括：

- 常见描述符
    设备描述符：描述设备的厂商 ID、产品 ID、设备类等。

    配置描述符：描述设备的配置信息。

    接口描述符：描述接口的类型、端点和协议。

    端点描述符：描述端点的地址、方向和最大数据包长度。

- 举例
    设备描述符：设备向主机报告其厂商 ID 和产品 ID。

    接口描述符：设备向主机报告其接口类型（如 HID 接口）。

    端点描述符：设备向主机报告其端点的地址和方向。

## 4.8 USB 通信流程
以下是 USB 通信的基本流程：

- 设备连接：设备连接到主机，主机检测到设备并开始枚举过程。

- 设备枚举：主机获取设备的描述符，配置设备并分配地址。

- 数据传输：主机和设备通过管道进行数据传输。

- 设备断开：设备从主机断开，主机释放资源。

# 五、总结
- 理解USB相关概念
    USB 规范：定义了 USB 技术的标准。
    USB 接口：设备与主机通信的逻辑通道。
    USB 端点：设备上的数据缓冲区。
    USB 管道：主机和设备之间的逻辑通道。
    USB 数据传输类型：控制传输、中断传输、批量传输、同步传输。
    USB 设备类：定义了设备的功能和通信协议。
    USB 描述符：设备向主机报告其配置和功能的数据结构。
- USB在日常生活常见是因为“通用”，USB通信协议介于物理层通信协议(IIC UART SPI等)和OSI参考模型（以太网）之间


# 六、讨论或思考
- USB和CAN两种协议对比相同点和区别？
- HID是否比CDC抗干扰？