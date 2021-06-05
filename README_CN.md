<h1 align = "center">🌟 LilyGo-EPD47-HC08-MIC-Test 🌟</h1>

## **中文 | [English](./README.md)** 

## 一、简介
1、这是一个关于测试 HC08-PDM_MIC 低功耗蓝牙模组的Dome.  

 -  将 HC08-PDM_MIC 模组安装于一块 LilyGo-EPD47 水墨屏上
    
 -  低功耗蓝牙模块HC-08处于串口透传模式，让暂时不需要工作的LilyGo-EPD47水墨屏中的ESP32进入休眠模式，使电池的损耗减到最低。
   
 -  您可以随时唤醒您的LilyGo-EPD47，通过另外一块蓝牙模组连接后发送串口字符，唤醒ESP32让其恢复工作 
   
 -  HC08-PDM_MIC 模组上还附有一颗PDM麦克风，可供进行语音方面的开发，DOME会测试它是否正常工作。  
  
## 二、硬件准备
1、HC08-PDM_MIC 蓝牙模组  
2、USB 转 TTL 串口模块  
3、LilyGo-EPD47墨水屏  
4、任意的 ESP32 开发板  
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
AT+LED=0           //关闭LED灯，省电
AT+NAME = INK_047  //蓝牙名称改为 INK_047
**AT+MODE = 1        //一级节能模式（必要）**  
  
![HC08-ATset](/images/ATset.jpg)  
  
  
## 四、硬件安装
1、在 LilyGo-EPD47 上安装 HC08-PDM_MIC 模组  
![#f03c15](**（注意：必须断电操作，否则将有可能烧坏水墨屏ESP32模组，后果自负！！）**  )
![HC08install](/images/2.jpg)

  
  
## 五、程序
### 1、LilyGo-EPD47   
 -  用 ArduinoIED 打开 \examples\epd47_show\epd47_show.ino  
   
 -  **开发板选择请"TTGO T-Watch"**(否则会因为项目占用储存过大导致编译失败)  
   
 -  选择正确的端口后即可编译上传,上传成功后，等待片刻直至墨水屏显示以下文本即为成功  
  
![EPD47show1](/images/4.jpg)  
"sleep began, wait for wake up"  
  
  
-  当程序开始时，LilyGo-EPD47水墨屏会自动进入休眠模式，这时整体电流将降到**1mA**或者更低
![EPD47_Sleep](/images/10.jpg)


### 2、EPS32蓝牙按键唤醒  
 -  使用 ArduinoIED 打开  \examples\epd47_blue_waker_center\epd47_blue_waker_center.ino  
   
 -  **在编译之前请确认您的按钮引脚，代码中选用的按钮引脚为15**  
   
 -  选择正确的端口后即可编译上传,上传成功后，按下您设置的按钮，等待片刻会出现以下画面 
  
 -  这时，ESP32通过蓝牙串口给HC-08发送了特定的唤醒字符，让LilyGo-EPD47中的ESP32恢复正常工作状态，可以正常完成它的工作后选择再次进入休眠 

![EPD47show2](/images/11.jpg) 
"Please blow into the microphone"  
  
  
  
## 六、测试麦克风
 -  这个测试程序在ESP32唤醒后会测试麦克风是否正常工作 

 -  当水墨屏显示：“请对麦克风吹气”时，请对以下部件吹气 
  
![PDM_MIC](/images/6.jpg) 
  
  
 -  当麦克风检测到有气压输入后,ESP32会再次进入休眠状态，等待再次激活  
  
![EPD47show3](/images/12.jpg) 
"microphone test normally, sleep began"  
  
   
## 七、运行演示视频
https://github.com/Cai1902126141/LilyGo-EPD47/blob/main/DemoVideo.mp4?raw=true  
  
## 八、补充说明
**1、HC-08模块任何拔插操作务必先断电！**  
**2、HC-08模组必须设置为一级节能模式，否则程序无法休眠，请确保AT指令设置成功**  
 
  