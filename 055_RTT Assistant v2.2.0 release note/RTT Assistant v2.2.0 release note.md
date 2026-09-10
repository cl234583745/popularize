嵌入式科普(55)RTT Assistant v2.0.0 → v2.2.0 更新汇总


经过 v2.1.0、v2.1.1、v2.1.2、v2.1.5、v2.2.0 共5个版本的迭代（基于v2.0.0），以下是所有新增功能、Bug修复和优化的汇总。

一、新增功能
1. 示波器模式（v2.1.0 起可用，v2.2.0 增强子通道）
功能
版本
说明
J-Scope数据格式
v2.1.0
自动识别7种固定格式：int8/uint8/int16/uint16/int32/uint32/float
采集控制
v2.1.0
开始/停止/暂停/恢复采集；设备断开自动停止
高速模式自动切换
v2.1.0
<2000样本/s普通模式，>5000样本/s自动切换高速模式（独立QThread + 30FPS降采样）
硬件时间戳支持
v2.1.0
支持带硬件时间戳的JScope数据包解析和波形显示
子通道拆分显示
v2.2.0
JScope合并buffer模式（如JScope_u4u4）自动拆分为独立子通道，命名格式CH1[1]、CH1[2]
2. 烧录固件（v2.2.0）
功能
说明
工具栏烧录按钮
一键烧录固件到MCU，支持J-Link(pylink)/DAP-Link/ST-Link(pyocd)
固件文件管理
配置面板支持多条路径、打开/替换/删除操作
烧录进度窗口
实时日志、成功绿色/失败红色标签、2秒自动关闭
固件与Map联动
固件路径与RTT map文件路径双向联动
错误提示
文件不存在/调试器未配置/格式不支持等弹窗提示
3. 主题与界面（v2.1.0）
功能
说明
暗色/亮色主题切换
支持主题切换，暗色模式下禁用状态显示更清晰（虚线边框+暗色文字）
4. 国际化 i18n（v2.1.2 起，v2.2.0 增强）
功能
版本
说明
中英文切换
v2.1.2
基础i18n框架支持语言切换
烧录UI国际化
v2.2.0
烧录UI（工具栏/配置面板/进度窗口）支持语言切换，16个i18n翻译键值
Pack弹窗国际化
v2.1.5+
EN/CN按钮逻辑修正、调试器选中色适配、pack列暗色可见性修复
5. 其他新增
功能
版本
说明
检查新版本
v2.1.0
自动或引导下载新版本
自适应轮询策略
v2.1.5
有数据时快速轮询(2ms)，空闲时降频(10ms)，多通道动态计算
CH0日志UI节流
v2.1.5
50ms定时器批量合并插入，性能提升约7-10倍
智能滚动跟踪
v2.1.5
滚动条在底部自动跟踪，手动上拉停止跟踪，拖回底部自动恢复
诊断日志管理
v2.1.5
日志菜单新增"日志管理"，可查看/清空3个诊断日志
按键效果优化
v2.1.5
暗色/亮色模式下按钮增加边框，按压有下沉反馈
Pack直接下载
v2.1.2
urllib直接下载.pack文件到runtime/packs/，显示进度和下载链接
cpm_cache本地化
v2.1.2
缓存从%LOCALAPPDATA%改为runtime/cpm_cache/，新电脑无需联网即可搜索Pack
pyvenv.cfg自动修复
v2.1.2
启动时自动检测修复无效路径，项目移动后无需手动修改
RTT通道名和缓冲区大小显示
v2.1.1
接收区/发送区GroupBox标题显示通道名和MCU缓冲区大小
配置持久化完善
v2.2.0
config.json启动时生成，缺失字段补全，窗口尺寸/最大化/工具栏位置保存恢复
环形缓冲区满告警
v2.1.0
RingBuffer写满时通过error_occurred信号发出告警（含丢弃字节数和通道号），带防刷屏逻辑
二、Bug修复
关键修复
修复项
版本
说明
RTT日志行丢失
v2.1.5
pyocd后端ch.read(length=1024)硬编码截断，改为无参数调用一次读完所有可用数据；JLink后端默认4096+积压读取循环
多通道+硬件时间戳波形乱套
v2.1.1
双路分发导致时间戳和值重复，改为batch_received单一路径；JScope包跨RTT读取边界截断，增加残缺包缓冲区
示波器子通道波形显示
v2.2.0
JScope_u4u4等多字段格式只显示1条波形，引入SubChannelSplitter拆分为独立子通道
Pack自动下载失败（打包后致命Bug）
v2.1.2
subprocess调用venv python因pyvenv.cfg无效路径失败，改为in-process import + urllib直接下载
无探针时点击连接闪退
v2.1.1
JLink DLL原生弹窗在QThread中触发段错误，禁用DLL弹窗+抛明确错误
J-Link断开后重连失败
v2.1.0
disconnect未置空wrapper，断开时未强制清理残留资源
一般修复
修复项
版本
说明
UI接收区滚动条强制拉到底部
v2.1.5
改用QTextCursor直接操作文档插入，仅在底部时自动滚动
环形缓冲区满警告刷屏
v2.1.5
降级为debug日志，不影响UI显示
高速模式频率/周期计算错误
v2.1.1
降采样前对原始数据计算频率，各通道频率文本按索引垂直偏移
硬件时间戳基准跨通道共享错位
v2.1.1
改为每通道独立的_hw_ts_origins[channel]字典
缓冲数据读取非原子
v2.1.1
截取min(len(ts), len(vals))保证返回长度一致，消除deque竞态窗口
频率乱闪
v2.2.0
限制最近500样本+每10次flush才重算
UnboundLocalError
v2.2.0
提取_resolve_channel_key()辅助方法消除局部导入遮蔽
快速连接时未配置探针无提示
v2.1.1
立即提示用户去配置页面刷新探针
Pack下载断网误报成功
v2.1.2
下载后验证.pack文件是否实际存在
Pack下载无进度显示
v2.1.2
下载对话框显示链接、文件大小、10%粒度进度
依赖管理窗口打包后卡顿
v2.1.2
Python版本用sys.version_info获取，包版本用importlib.metadata读取
config.json硬编码开发机路径
v2.1.2
map_file_path默认值改为null
pyocd烧录.srec格式
v2.2.0
自动替换为同名.elf文件
Down buffer信息读取
v2.1.1
发送区显示MCU未配置Down缓冲区提示
RTT CB地址签名验证
v2.1.1
auto模式搜索增加验证，避免搜到内存中碰巧含SEGGER RTT字节的错误地址
Worker线程终止
v2.2.0
烧录线程正确终止(abort+wait)
三、性能优化
优化项
版本
效果
分阶段加载启动
v2.1.0
窗口显示速度从1200ms优化至450ms，首次点击不再白屏
移除QThread.terminate()
v2.1.0
改用协作式退出，避免J-Link DLL状态损坏和USB句柄泄漏
错误日志修正
v2.1.0
J-Link后端不再打印误导性pyocd rtt命令行，改为输出正确的JLinkRTTLogger命令
Pack操作去掉subprocess
v2.1.2
搜索0ms（字典搜索），下载用urllib直接下载
依赖版本读取
v2.1.2
importlib.metadata读元数据代替import整个包（快100倍）
Signal合并
v2.1.5
同一轮询周期内多次CH0读取合并为1次data_received信号发射
四、界面优化（v2.1.0）
优化发送区宽度、位置和发送换行提示
优化收发计数重置按钮位置
优化定制按钮位置
优化字体修改后保存
优化状态栏连接状态提醒色
五、打包优化
优化项
版本
说明
pyqtgraph+numpy打包进exe
v2.1.0
示波器开箱即用，无需系统安装
venv移除冗余包
v2.1.0
移除PyQt5(142MB)/pyqtgraph(7MB)/numpy(30MB)，减小分发体积约179MB
shebang/pyvenv.cfg修复
v2.1.2
build.py自动修复venv Scripts的shebang和pyvenv.cfg路径
cpm_cache索引复制
v2.1.2
build.py复制index.json到分发目录
六、功能演进概览

七、新增配置项汇总
配置项
默认值
版本
说明
ring_buffer_size	
65536
v2.1.5
环形缓冲区大小
ring_buffer_full_log_level	
DEBUG
v2.1.5
缓冲区满警告级别
log_level.rtt_system	
INFO
v2.1.5
rtt_system日志级别
log_level.pyocd_diag	
INFO
v2.1.5
pyocd_diag日志级别
log_level.rtt_debug	
INFO
v2.1.5
rtt_debug日志级别
firmware_paths	
[]
v2.2.0
固件文件路径列表
active_firmware_index	
0
v2.2.0
当前激活的固件路径索引
pylink_read_size	
4096
v2.1.5
JLink后端RTT读取缓冲区大小

迅雷下载 GitHub: 
https://github.com/cl234583745/RTT-Assistant/releases/download/v2.2.0/RTT-Assistant.v2.2.0.7z 

CSDN: 
https://download.csdn.net/download/cl234583745/92870951  

gitee: 
https://gitee.com/292812832/RTT-Assistant/releases/download/v2.2.0/RTT-Assistant%20v2.2.0.7z.001  
https://gitee.com/292812832/RTT-Assistant/releases/download/v2.2.0/RTT-Assistant%20v2.2.0.7z.002