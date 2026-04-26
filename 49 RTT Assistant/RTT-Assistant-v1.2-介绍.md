嵌入式科普(49)RTT Assistant v1.2 功能介绍
===
[toc]

# 一、概述

RTT Assistant是一款基于JLink RTT的MCU调试工具，通过SWD/JTAG接口实现PC与MCU的高效通信。
https://github.com/cl234583745/Segger-RTT-Assistant/releases/download/v1.2/dist.zip

https://gitee.com/292812832/Segger-RTT-Assistant/blob/main/dist/RTT-Assistant-v1.2.exe

**exe文件**：`dist/RTT-Assistant-v1.2.exe`

# 二、核心功能

| 功能 | 说明 |
|------|------|
| RTT通信 | 通过JLink连接MCU，实现数据收发 |
| 连接配置 | USB/TCP/IP连接，可配置设备型号、接口速度 |
| 数据接收 | HEX/字符串显示，时间戳显示 |
| 数据发送 | 字符串/HEX模式，自动换行 |
| 日志记录 | 独立日志窗口，类型过滤 |
| 配置保存 | 自动保存设置 |

# 三、对比JLink RTT Viewer

## 功能对比

| 功能 | JLink RTT Viewer | RTT Assistant |
|------|:----------------:|:-------------:|
| 数据收发 | ✅ | ✅ |
| HEX显示 | ✅ | ✅ |
| **时间戳显示** | ❌ | ✅ |
| **日志窗口** | ❌ | ✅ |
| **配置保存** | ❌ | ✅ |
| **数据导出** | ❌ | ✅ |
| **独立exe** | ❌ | ✅ |
| **中文界面** | ❌ | ✅ |
| 设备选择 | ✅ | ✅ |
| RTT地址配置 | ✅ | ✅ |

## 核心优势

| 优势 | 说明 |
|------|------|
| **便携性** | 单exe文件，无需安装Python环境 |
| **易用性** | 中文界面，操作简单直观 |
| **时间戳** | 精确记录数据收发时间，便于调试分析 |
| **日志追踪** | 记录所有连接和通讯事件，问题排查更高效 |
| **配置记忆** | 自动保存设置，下次启动自动恢复 |
| **数据导出** | 一键导出接收数据到文件 |

# 四、使用方法

## 4.1 运行exe

```
双击 dist/RTT-Assistant-v1.2.exe
```

## 4.2 连接MCU

1. 点击"连接"按钮
2. 配置连接参数：
   - 连接方式：USB/TCP/IP
   - 设备型号：如Cortex-M4、R9A07G084M04等
   - 接口类型：SWD/JTAG
   - 接口速度：建议4000 kHz
   - RTT控制块：通常选择"自动检测"
3. 点击"确定"连接

## 4.3 数据收发

- **接收**：数据自动显示，可切换HEX/字符串模式，可开启时间戳
- **发送**：输入数据，选择模式，点击"发送"或按回车

# 五、系统要求

- Windows 7/10/11
- JLink软件（V930+）
- MCU已移植RTT代码

# 六、附录

## 版本历史

| 版本 | 更新内容 |
|------|---------|
| v1.2 | 日志窗口、瑞萨设备支持、RTT地址保存 |
| v1.1 | 连接配置对话框、pylink集成 |
| v1.0 | 基础RTT收发功能 |

## 相关链接

- SEGGER RTT：https://www.segger.com/products/debug-probes/j-link/tools/about-real-time-transfer/
- JLink下载：https://www.segger.com/downloads/jlink/
