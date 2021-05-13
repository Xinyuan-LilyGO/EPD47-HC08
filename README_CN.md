<h1 align = "center">🌟 LilyGo-EPD47-HC08-PDM_MIC-Test 🌟</h1>

## **中文 | [English](./README.md)** 

## 一、简介
1、这是一个关于测试 HC08-PDM_MIC 低功耗蓝牙模组的Dome. 
 -  将 HC08-PDM_MIC 模组安装于一块 LilyGo-EPD47 水墨屏上,并将其休眠  
   
 -  通过另外一块 ESP32 随时唤醒正在休眠的 LilyGo-EPD47 发送需要显示的字符文本,并使其进入麦克风检测  
   
 -  当检测到有声音后再次进入休眠.按下定义的按钮可重复以上测试步骤.  
  
## 二、硬件准备
1、HC08-PDM_MIC 蓝牙模组  
2、USB 转 TTL 串口模块  
3、LilyGo-EPD47  
4、ESP32 开发板  
5、按钮
6、FPC排线 6 Pin   
  
![Hardware display](/images/1.jpg)  
  
  
## 三、HC08 AT设置
1、HC08 连接 USB 转 TTL 串口模块  
  
+-----------------------+   
|HC08--------USBtoTTL|  
|TX-------------------RX|  
|RX-------------------TX|  
|VCC----------------3.3V|  
|GND--------------GND|  
+-----------------------+  
  
2、发送AT指令设置蓝牙模组  
AT+LED=0           //关闭LED灯，设置成功收到返回字符：OK+LED=0  
AT+NAME = INK_047  //蓝牙名称，设置成功收到返回字符：OKsetNAME:INK_047  
**AT+MODE = 1        //一级节能模式（必要），设置成功收到返回字符：OK**  
  
![HC08-ATset](/images/ATset.jpg)  
  
  
## 四、硬件安装
1、在 LilyGo-EPD47 上安装 HC08-PDM_MIC 模组  

![HC08install](/images/2.jpg)
**（注意：必须断电操作，否则有可能烧坏水墨屏ESP32模组）**  
  
  
## 五、烧录固件
### 1、LilyGo-EPD47 固件  
 -  用 ArduinoIED 打开 \examples\epd47_show\epd47_show.ino  
   
 -  **开发板选择请"TTGO T-Watch"**(否则会因为项目占用储存过大导致编译失败)  
   
 -  选择正确的端口后即可编译上传,上传成功后，等待片刻直至墨水屏显示以下文本即为成功  
  
![EPD47show1](/images/4.jpg)  
"sleep began, wait for wake up"  
  
  
### 2、EPS32 开发板固件  
 -  使用 ArduinoIED 打开  \examples\epd47_blue_waker_center\epd47_blue_waker_center.ino  
   
 -  **在编译之前请确认您的按钮引脚，代码中选用的按钮引脚为15**  
   
 -  选择正确的端口后即可编译上传,上传成功后，等待10秒左右,直至墨水屏闪屏显示以下文本  

![EPD47show2](/images/5.jpg) 
"Please blow into the microphone"  
  
  
## 六、测试步骤
1、 当水墨屏显示：“请对麦克风吹气”时，请对以下部件吹气 
  
![PDM_MIC](/images/6.jpg) 
  
  
2、当麦克风检测到有气压输入后,ESP32进入休眠状态，等待再次激活  
  
![EPD47show3](/images/7.jpg) 
"microphone test normally, sleep began"  
  
  
3、如再次重复测试，可按下按钮，等待墨水屏(5秒左右)闪烁显示文本后，即可重复测试步骤  
  
![ButtonPress](/images/8.jpg) 
  

## 七、运行演示视频
https://github.com/Cai1902126141/LilyGo-EPD47/blob/main/DemoVideo.mp4?raw=true  
  
## 八、补充说明
**1、HC-08模块任何拔插操作务必先断电！**  
**2、HC-08模组必须设置为一级节能模式，否则程序无法休眠，请确保AT指令设置成功**  
3、测试前请确认HC-08麦克风模组与墨水屏插稳  
4、如按下按钮后，墨水屏没有响应，可尝试按钮按大力按久一点，确保按钮触发  
5、按钮唤醒水墨屏后，必须先完成麦克风测试步骤，否则会一直等待麦克风输入不会休眠  
  

