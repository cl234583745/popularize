嵌入式科普(24)从SPI和CAN通信重新理解“全双工”
===
[toc]
# 一、概述
- 前段时间因为没有真正理解“全双工”的概念，导致spi的驱动写了好几天
- 大家知道spi是全双工；can是全双工/半双工争议非常多
- 究竟什么是“全双工/半双工”？有没有标准？
- 真正理解这个概念的好处：正确的编写驱动函数；深刻理解才能思路清晰

# 二、参考资料
- https://www.bosch-semiconductors.com/zh/ip%E6%A8%A1%E5%9D%97/can-%E5%8D%8F%E8%AE%AE/
- [IEEE802.3-2018.pdf](./Doc/IEEE802.3-2018.pdf)
- ![](./images/ieee802.3%20full%20duplex.png)
- [CAN入门书.pdf](./Doc/CAN入门书.pdf)
- [STM32H723/733 Reference manual](./Doc/DM00603761_ENV2.pdf)
- ![](./images/stm32%20spi%20full%20duplex.png)
- https://blog.csdn.net/weixin_43746325/article/details/119837354

# 三、嵌入式常见的通信对比



| 通信协议 | 类型 | 线数 | 传输模式 | 使用场景 |
| --- | --- | --- | --- | --- |
| I²C | 半双工 | SDA<---><br>**SCL --->** | 单主多从|低速芯片间通信 |
| UART | 全双工 | TX---><br>RX<---| 点对点 | 芯片间通信，debug接口 |
| **SPI** | **全双工** | MISO<---<br>MOSI---><br>**SCLK--->**<br>CS---> | 单主多从| 高速同步芯片间通信 |
| USB | USB2.0半双工<br>USB3.0全双工 |差分:<br>D+<br>D-| 主从 | PC热插拔 |
| **CAN** | **半双工** |**控制器: \ 收发器:**<br>RXD \ CAN_H<br>TXD \ CAN_L| 多主 | 汽车和工业自动化 |
| 以太网 | 全双工 |**PHY:**<br>TX+<br>TX-<br>RX+<br>RX-| 多主 | 局域网，嵌入式设备 |

 **关键：**
- **SPI**的时钟SCLK是主发给从，是单向的；但数据线是两根，是双向的
- **CAN**的物理层是2跟差分信号，是单向的；但控制器是tx rx两根，是双向的
 
# 四、驱动函数对比
| 通信协议 | API函数 |
| --- | --- |
| I²C |HAL_I2C_Master_Transmit<br>HAL_I2C_Master_Receive|
| UART |HAL_UART_Transmit<br>HAL_UART_Receive|
| **SPI** |HAL_SPI_Transmit<br>HAL_SPI_Receive<br>HAL_SPI_TransmitReceive|
| **CAN** |HAL_FDCAN_AddMessageToTxFifoQ<br>HAL_FDCAN_GetRxMessage|

# 五、总结
- **全双工**指**物理介质**能够在没有干扰的情况下支持**同步传输和接收**。
- **CAN的本质是半双工**，因为差分信号只能单向通信；所谓CAN在协议层是全双工，实际是FIFO、仲裁机制等实现
- **SPI**有**收发两根**数据线，可以同时收发，满足全双工，但时钟是主发给从单向，所以从无法主动发送数据，从只能等待主发送时“顺带”发送。类似zigbee休眠的poll机制，收发数据的时机是同时发生的，**也算全双工**
- 从SPI的API的命名（**HAL_SPI_TransmitReceive**）也能发现，所以写spi的驱动函数与其他协议是不同的。