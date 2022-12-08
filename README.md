## 三根数据线&1.6元&超小PCB布局&超低功耗。可以让你的设备获得手机一般的电源管理。
 

2022.12.8更新加入了菜单功能，按键和电池信息可以通过菜单切换！！！！
其它详细信息见开源广场：https://oshwhub.com/mondraker/axp173

代码更新基本在GitHub完成，硬件方面主要更新在开源工程里！！！
请注意：Arduino 不支持 "printf", 请全部替换为 "print" 或 "println"


以下是正文：


1.本库寄存器读写方式主要借鉴了m5stack官网的AXP192库。IIC_PORT库来自 Forairaaaaa。该芯片的通用版本感谢 liuzewen指路，否则由于该芯片的定制特性导致默认输出不一样IIC无法通讯。


2.自己注释了所有函数！！！以及编写了其它自己需要的函数。比如通过识别IRQ中断引脚与中断寄存器设置长按关机与短按息屏等等，还可以自己设置息屏后关闭哪些输出等等。

 
3.你可以用它做：蓝牙键盘的充电管理，开发板的充电管理，微型手表的充电管理，小电视的充电管理……等等等等等等等等


4.使用方法：见接线图与底部视频演示（请先参阅程序以及数据手册，人多了会抽时间出视频教程）


5.其它：文档后续会持续完善，最近比较忙。不懂得可以先来QQ群问：735791683
 

### arduino，esp8266，esp32等等直接无需移植，直接拿来即用。可以直接打包成文件夹放入Arduino IDE的lib库中（具体操作见开源简介）。已经添加关键字文件，使用函数与关键字支持代码高亮。


#### 芯片基本功能（基本榨干常用功能）

![功能](https://image.lceda.cn/pullimage/kahHHHXHiTKpMsMp9WOTKofT0jKPpmMOKs7hHRJN.png)

#### 详情见：https://oshwhub.com/mondraker/axp173

#### 这里只有代码（以下是接线框图）

![具体接线图](https://image.lceda.cn/pullimage/0UmiCtTcMNNo9QroSF73lCTjk80fvxGcqTW8h9C7.png)

#### 一张图片

![芯片](https://image.lceda.cn/pullimage/HB7w1x4u9ayl66i4vpHSZOuA4biLDCnlTWlBK2qN.jpeg)
