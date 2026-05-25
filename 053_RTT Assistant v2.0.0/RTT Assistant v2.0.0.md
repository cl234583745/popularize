v2.0.0 更新内容 (最新)
架构重构
Runtime目录分离
所有运行时文件从exe同级目录迁移到runtime/子目录
runtime/dll/
：JLink_x64.dll、libusb-1.0.dll
runtime/packs/
：CMSIS Pack文件
runtime/venv/
：Python虚拟环境（PyOCD及依赖）
配置文件迁移到config/目录
exe可完全独立，不污染用户目录

独立Python虚拟环境

PyOCD及所有依赖安装在runtime/venv/中
不依赖系统Python环境
首次使用可通过"帮助→依赖管理"安装依赖
新增功能
支持ST-Link连接
使用NUCLEO-U575ZI-Q验证ST-Link RTT通讯
使用RT-Thread Titan Board验证DAP-Link RTT通讯
使用EK-RA8P1验证J-link ob RTT通讯

性能优化
日志轮转：rtt_system.log和pyocd_diag.log使用RotatingFileHandler（5MB轮转）
PyOCD目标索引：首次从pyocd_targets.txt快速加载（毫秒级），后台异步刷新
打包构建：build.py自动终止占用exe的进程，venv复制完整性验证
打包改进
exe命名前缀RTT-（RTT-Assistant v2.0.0.exe）
build.py：打包前清空dist、exe占用时自动taskkill、venv复制关键包验证+重试
RTT-Assistant.spec：excludes排除usb/usb1/pyocd/hid，runtime_hook用meta path finder hack绕过

迅雷下载 GitHub:
https://github.com/cl234583745/RTT-Assistant/releases/download/v2.0.0/RTT-Assistant.v2.0.0.7z
CSDN:
https://download.csdn.net/download/cl234583745/92870951

gitee:
https://gitee.com/292812832/RTT-Assistant/releases/download/v2.0.0/RTT-Assistant%20v2.0.0.7z.001

https://gitee.com/292812832/RTT-Assistant/releases/download/v2.0.0/RTT-Assistant%20v2.0.0.7z.002