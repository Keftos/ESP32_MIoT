 *接入米家实现小爱同学控制led
 
 *使用Blinker平台，手机app按钮控制
 
 *PIN32触摸开关LED
 
 *OLED显示网络时间和引脚变化状态

 # 安装环境
 
 本人刚接触ESP32使用的是Arduino进行开发，使用了两天受不了编译慢的问题

 转战vscode + PlatformIO 进行开发

 ### 安装PlatformIO

 *首先打开vscode-->点击左侧拓展-->搜索PlatformIO-->进行安装（建议先安装python，否则可能卡pip，并且挂上梯子）

 *安装完成后左侧显示一个蚂蚁图标

 *点击蚂蚁图标-->新建工程-->选择“Espressif ESP32 Dev Module”开发板-->反选“Location”进行项目保存地址选择-->点击“Finish”开始下载库文件

 **Attention 1：中国大陆环境这里下载ESP32官方库文件需要很长时间，建议挂全局梯子**

 **Attention 2：挂梯子也不行的话可以试试这个方法[VSCode PlatformIO下载慢、新建项目速度慢解决方法](https://www.bilibili.com/read/cv34484885/) 我就是用这个方法下好的，下了两分钟左右**

 ![新建工程](https://png.ueat.top/i/2024/06/26/saekox.png) 

 # 烧录源码

 美美过上智能家居生活


 

 
