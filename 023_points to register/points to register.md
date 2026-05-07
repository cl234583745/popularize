嵌入式科普(23)指向寄存器的指针变量
===
[toc]
# 一、概述
- 通过一个驱动代码来分析其中的bug或风险

# 二、串口轮询发送函数
```
uint32_t USR_SCI_UART_Write(sci_uart_instance_ctrl_t *p_ctrl,
                            const sci_uart_api_ctrl_t *p_api_ctrl,
                            const uint8_t *p_src,
                            uint32_t bytes)
{ 
  sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;  
  volatile uint32_t *reg = (uint32_t *) &p_ctrl->p_reg->SSR;  
  uint8_t *data = (uint8_t *) p_src;  
  for(uint32_t i = 0; i < bytes; i++)  
  {  
    p_ctrl->p_reg->TDR_b.TDR = *data;  
    while(((*reg >> 7) & 0x1) == 0){  
    }  
    data++;   
  } 
}
```
- 实例化便于跨平台
- mpu移植到mcu
- mpu状态寄存器32bit，mcu状态寄存器8bit
- 指针变量可以指向任意地址不报错
- 等待状态寄存器的bit置位

# 三、优化
```
uint32_t USR_SCI_UART_Write(sci_uart_instance_ctrl_t *p_ctrl,
                            const sci_uart_api_ctrl_t *p_api_ctrl,
                            const uint8_t *p_src,
                            uint32_t bytes)
{ 
  sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;  
  uint8_t *data = (uint8_t *) p_src;  
  for(uint32_t i = 0; i < bytes; i++)  
  {  
    p_ctrl->p_reg->TDR_b.TDR = *data;  
    while(((p_ctrl->p_reg->SSR) & (1 << 7)) == 0);
    //while(p_ctrl->p_reg->SSR_b.TDRE == 0);
    data++;   
  } 
}
```
# 四、总结
- 定义指向寄存器的指针变量要额外注意
- 编译器警告类型不匹配的不能简单忽略
- 强制类型转换不能随意用
- 位掩码，如 (*reg & (1 << n))
