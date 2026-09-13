嵌入式科普(57) 各类MCU/MPU的以太网功能详细对比
===
[toc]


# 1. 基础规格对比

| 芯片 | 类型 | CPU内核 | 以太网MAC数量 | MAC速率 | PHY接口 | 内置PHY |
|------|------|---------|--------------|---------|---------|---------|
| **STM32F407** | MCU | Cortex-M4 @ 168MHz | 1x EMAC | 10/100M | MII, RMII | - |
| **GD32H77R** | MCU | Cortex-M7 @ 600MHz | 2x 以太网 | 10/100M | MII | 有，2x 100M PHY |
| **RZN2L** | MCU | Cortex-M52 @ 400MHz | 1x GMAC  | 10/100/1000M | RGMII, RMII, MII | - |
| **HPM5E00** | MCU | RISC-V 双核 @ 480MHz | 1x ENET | 10/100/1000M | RGMII, RMII | 有，2x 100M PHY |
| **HPM6E80** | MCU | RISC-V 双核 @ 600MHz | 1x ENET | 10/100/1000M | RGMII, RMII, MII | 有，2x 100M PHY |
| **RA8P1** | MCU | Cortex-M85 @ 1GHz<br>Cortex-M33 @ 250MHz | 1x RMAC + ESWM 2端口 | RMAC: 10/100/1000/2500M; <br>ESWM: 10/100/1000M | RMAC: MII, GMII; <br>ESWM: MII, RMII, GMII, RGMII | - |
| **RA8T2** | MCU | Cortex-M85 @ 1GHz<br>Cortex-M33 @ 250MHz | 1x RMAC + ESWM 2端口 | RMAC: 10/100/1000/2500M; <br>ESWM: 10/100/1000M | RMAC: MII, GMII; <br>ESWM: MII, RMII, GMII, RGMII | - |
| **RK3506** | MPU | Cortex-A7 三核 @ 1.5GHz | 2x RMII MAC | 10/100M | RMII | - |
| **RK3588** | MPU | Cortex-A76 四核@ 2.4GHz<br>Cortex-A55 四核@ 1.8GHz | 2x GbE | 10/100/1000M | - | - |
| **RZ/T2H** | MPU | Cortex-A55 四核@ 1.2GHz<br>Cortex-R52 双核@ 1GHz | 3x GMAC + 4个共享MAC-PHY接口 | 10/100/1000M | RGMII, RMII, MII | - |



# 2. 交换机功能对比

| 芯片 | 交换机类型 | 外部端口 | 交换容量 | MAC表深度 | VLAN表 | 备注 |
|------|-----------|---------|---------|----------|--------|------|
| **STM32F407** | - | - | - | - | - | - |
| **GD32H77R** | - | - | - | - | - | - |
| **HPM5E00** | - | - | - | - | - | - |
| **RK3506** | - | - | - | - | - | - |
| **RK3588** | - | - | - | - | - | - |
| **RZN2L** | **ETHSW (L2)** | 3个外部 + 1个内部 | 3Gbps | 4K | 32 | 支持TDMA/DLR/PRP |
| **HPM6E80** | **TSW (L2)** | 3个外部 + 1个CPU内部 | - | 128 | - | 每端口8优先级队列；VLAN tagging |
| **RA8P1** | **ESWM (L3)** | 2个外部 | - | 2048 | 4096 | 支持L3 stream-ID转发 |
| **RA8T2** | **ESWM (L3)** | 2个外部 | - | 2048 | 4096 | 同RA8P1 |
| **RZ/T2H** | **ETHSW (L2)** | 3个外部 + 1个内部 | 3Gbps | 4K | 32 | 与RZN2L交换机规格基本相同 |

# 3. L2/L3 转发能力对比

| 芯片 | L2交换 | L3路由 | 流表 | L2/L3更新规则 | CPU可介入转发 |
|------|--------|--------|------|--------------|-------------|
| **RZN2L ETHSW** | ✅ 硬件线速 | - | - | - | - |
| **HPM6E80 TSW** | ✅ 硬件线速 | - | - | - | 通过CPU内部端口 |
| **RA8P1 ESWM** | ✅ 硬件线速 | ✅ 基于stream ID | ✅ 256条 | ✅ 256条 | ✅ GWCA（128bit AXI） |
| **RA8T2 ESWM** | ✅ 硬件线速 | ✅ 基于stream ID | ✅ 256条 | ✅ 256条 | ✅ GWCA（128bit AXI） |
| **RZ/T2H ETHSW** | ✅ 硬件线速 | - | - | - | - |

> **注意**：RA8P1/RA8T2 的 L3 转发是基于 stream ID 的硬件转发，不等于完整的 IPv4/IPv6 路由器、NAT 或通用 Linux 网络栈。

# 4. TSN（时间敏感网络）功能对比





| 功能 | RZN2L ETHSW | HPM6E80 TSW | RA8P1/RA8T2 ESWM | RZ/T2H ETHSW |
|------|-------------|-------------|------------------|--------------|
| **802.1AS-Rev** (gPTP) | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 |
| **802.1Qbv** (时间感知调度) | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 |
| **802.1Qbu/802.3br** (帧抢占) | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 |
| **802.1Qch** (循环排队) | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 |
| **802.1Qcr** (异步流量整形) | - | - | ✅ 已确认 | - |
| **802.1Qci** (流过滤) | - | ✅ 仅CPU端口侧 | ✅ 已确认(无gate filter) | - |
| **802.1CB** (FRER) | ✅ 已确认 | ✅ 仅CPU端口侧 | ✅ 已确认 | ✅ 已确认 |
| **802.1Qav** (CBS) | - | ✅ 已确认 | ✅ 已确认 | - |
| **IEEE 1722** (AVB) | - | - | ✅ 已确认 | - |
| **TDMA时隙调度** | ✅ 已确认 | - | - | ✅ 已确认 |
| **PRP/HSR** | ✅ 已确认 | - | - | ✅ 已确认 |
| **DLR** | ✅ 已确认 | - | - | ✅ 已确认 |
| **PSFP** | - | - | ✅ 已确认 | - |
| **TrustZone安全过滤** | - | - | ✅ 已确认 | - |



# 5. GMAC 高级特性对比


| 特性 | STM32F407 | RZN2L GMAC | RZ/T2H GMAC | RA8P1/RA8T2 RMAC |
|------|-----------|------------|-------------|------------------|
| **最大帧长** | 16KB | GMAC: 16KB; ETHSW: 10KB jumbo | GMAC: 16KB; ETHSW: 10KB jumbo | 64KB jumbo |
| **MAC地址过滤** | 4个完美DA + **64-bit Hash** | 17个地址寄存器 + 256-bit Hash | 32个地址寄存器 + 256-bit Hash + VLAN/L3/L4过滤 | 多种灵活RX过滤模式 |
| **DMA通道** | 1TX + 1RX | 1TX + 1RX | **8TX + 8RX** | - |
| **DMA FIFO** | 2KB TX + 2KB RX | 2KB TX + 4KB RX | **8KB TX + 8KB RX** | - |
| **多队列** | 不支持 | 不支持 | ✅ 8TX+8RX | - |
| **IEEE 1588时间戳** | ✅ v2, 64位 | ✅ v2 | ✅ v2 | ✅ v2 |
| **PTP时间戳卸载** | - | - | ✅ 已确认 | - |
| **TCP/IP卸载** | - | - | ✅ 已确认 | - |
| **ARP卸载** | - | - | ✅ 已确认 | - |
| **EEE (802.3az)** | - | ✅ 已确认 | ✅ 已确认 | ✅ 已确认 |
| **VLAN过滤** | ✅ 检测 | ✅ | ✅ L3/L4过滤 | ✅ |
| **流量控制** | ✅ 802.3x | ✅ 802.3br(由交换机支持) | ✅ 802.3x + 802.1Qbb + 802.3br | ✅ 802.3x + 802.1Qbb + 802.3br |
| **校验和卸载** | ✅ RX: IPv4/TCP/UDP/ICMP; TX: IPv4/TCP/UDP/ICMP | - | ✅ TCP/IP offload, ARP offload | - |

# 6. EtherCAT 从站控制器对比

| 芯片 | EtherCAT | 端口数 | PDRAM | 分布式时钟 | FMMU | SyncManager |
|------|----------|--------|-------|-----------|------|-------------|
| **RZN2L** | ✅ 已确认 | 3口 | 8KB | 64位 | 8 | 8 |
| **RA8T2** | ✅ 已确认 | 2口 | 8KB | 64位 | 8 | 8 |
| **HPM5E00** | ✅ 已确认 | 3口 | 60KB | 64位 | 8 | 8 |
| **HPM6E80** | ✅ 已确认 | 3口 | 60KB | 64位 | 8 | 8 |
| **RZ/T2H** | ✅ 已确认 | 3口 | 8KB | 64位 | 8 | 8 |
| **GD32H77R** | ✅ 已确认 | - | - | - | - | - |
| STM32F407 | - | - | - | - | - | - |
| RA8P1 | - | - | - | - | - | - |
| RK3506 | - | - | - | - | - | - |
| RK3588 | - | - | - | - | - | - |

# 7. ESWM（RA8P1/RA8T2）子模块架构

| 子模块 | 全称 | 功能描述 |
|--------|------|---------|
| **MFWD** | Message Forwarding Engine | 帧过滤、转发和路由引擎，支持L2/L3/端口转发、cut-through、PSFP、ATS、FRER |
| **COMA** | Common Agent | 公共管理功能，指针管理、APB-to-SFR总线转换、描述符拒绝处理 |
| **ETHA** | Ethernet Agent | ESWM与PHY之间的数据交换接口 |
| **RMAC** | Ethernet MAC | MAC层控制器，支持10/100/1000/2500Mbps |
| **GWCA** | CPU Agent | CPU接口，128bit AXI总线，支持out-of-order传输 |
| **GPTP** | Generic PTP Timer | 78位GPTP定时器 + 64/32位AVTP定时器 |

# 8. 总结
- 普通以太网MCU和高端MPU的网络接口区别是百兆->千兆MAC、1个->多个，跑openwrt就可以具备路由功能，路由功能是3层协议。
- 工业以太网MCU/MPU需要具体硬件ESC、L2/L3交换机，实现了无需CPU参与的数据通信转发。


| 类别 | 代表芯片 | 典型以太网架构 | 交换机 / EtherCAT | PHY情况 | 路由与软件特征 |
|------|----------|----------------|-------------------|---------|----------------|
| 普通以太网 MCU | STM32F407 | 百兆 MAC | - | - | - |
| 高端 Linux MPU | RK3506、RK3588 | 百兆或千兆 MAC | - | - | 可运行 OpenWrt；路由功能由三层软件协议实现，不依赖交换机硬件 |
| 一类工业以太网 MCU | HPM5E00、GD32H77R、XMC4800、RX72M | 百兆 MAC + EtherCAT ESC，ESC 通常为 2 至 3 个端口 | - | 可选内置 PHY | - |
| 二类工业以太网 MCU/MPU | RZN2L、HPM6E80、RA8T2 | 千兆 MAC + EtherCAT ESC + L2/L3 交换机 | 具备 L2/L3 交换能力；ESC 通常为 2 至 3 个端口 | 部分型号可集成 PHY | 具备有限的硬件三层转发能力 |
| 三类工业以太网 MPU | RZ/T2H | 多个千兆 MAC + EtherCAT ESC + L2/L3 交换机 | 具备 L2/L3 交换能力；ESC 通常为 2 至 3 个端口 | - | 可运行 OpenWrt；支持主从一体化架构和软件路由 |

