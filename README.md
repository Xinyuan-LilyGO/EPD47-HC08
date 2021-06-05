<h1 align = "center">🌟 LilyGo-EPD47-HC08-MIC-Test 🌟</h1> 

## **English | [中文](./README_CN.md)**  
  
## Introduction
### 1.This is a Dome about testing the HC08-PDM_MIC low energy Bluetooth module. 
   -  Install the HC08-PDM_MIC module on a LilyGo-EPD47 ink screen.  
     
   -  The Bluetooth low energy module HC-08 is in the serial port transparent transmission mode, so that the ESP32 in the LilyGo-EPD47 ink screen that does not need to work temporarily enters the sleep mode, so that the loss of the battery is minimized.  
     
   -  There is also a PDM microphone attached to the HC08-PDM_MIC module, which can be used for voice development, and DOME will test whether it is working properly.  
  
  
## Hardware preparation
1.HC08-PDM_MIC Bluetooth module  
  
2.USB to TTL serial port module  
  
3.LilyGo-EPD47  
  
4.ESP32 Development board  
  
5.Button  
  
6.FPC 6 Pin  
  
![Hardware display](/images/1.jpg)  
  
  
  
## HC08 AT Set
### 1.HC08 connects USB to TTL serial port module  
  
+-----------------------+   
|HC08--------USBtoTTL|  
|TX-------------------RX|  
|RX-------------------TX|  
|VCC----------------3.3V|  
|GND--------------GND|  
+-----------------------+  
  
  
### 2.Send AT command to set up Bluetooth module  
  
 - **AT+LED=0           //Turn off the LED,and return："OK+LED=0"**  
  
 - **AT+NAME = INK_047  //Change Bluetooth name,and return："OKsetNAME:INK_047"**  
   
 - **AT+MODE = 1**        //**Level 1 energy saving mode (required),and return："OK"**  
  
  
![HC08-ATset](/images/ATset.jpg)  
  
  
  
## Hardware installation
 - Install the HC08-PDM_MIC module on LilyGo-EPD47  
**(Warning: The power must be turned off, otherwise the ESP32 module of the screen may be burned out , At your own risk)**  
![HC08install](/images/2.jpg) 
  
  
  
## Upload firmware
### 1.LilyGo-EPD47 firmware  
 -  Open with ArduinoIED \examples\epd47_show\epd47_show.ino  
   
 -  **Please select the development board"TTGO T-Watch"**(Otherwise, the compilation will fail because the project occupies too much storage)  
   
 -  After selecting the correct port, you can compile and upload. After the upload is successful, wait for a while until the screen displays the following text, which means success  
  
![EPD47show1](/images/4.jpg)  
"sleep began, wait for wake up"  
  
  
 -  When the program starts, the LilyGo-EPD47 ink screen will automatically enter the sleep mode, then the overall current will drop to **1mA** or lower  
 ![EPD47_Sleep](/images/10.jpg) 
  
### 2. EPS32 Development board firmware  
 -  Open with ArduinoIED  \examples\epd47_blue_waker_center\epd47_blue_waker_center.ino  
  
 -  **Please confirm your button pin before compiling**，The button pin is 15  
  
 -  At this time, ESP32 sends a specific wake-up character to HC-08 through the Bluetooth serial port, so that the ESP32 in LilyGo-EPD47 can return to normal working state, and can choose to enter sleep again after completing its work normally 
  
![EPD47show2](/images/11.jpg) 
"Please blow into the microphone"  
  
  
  
  
## Test the PDM_MIC
### 1. When the screen displays：“请对麦克风吹气”,Please blow into the microphone
  
![PDM_MIC](/images/6.jpg) 
    

### 2.When the microphone detects air pressure, ESP32 enters the sleep state, waiting to be activated again  
  
![EPD47show3](/images/12.jpg) 
"microphone test normally, sleep began"  
  
  
  
## Demo video
https://github.com/Cai1902126141/LilyGo-EPD47/blob/main/DemoVideo.mp4?raw=true  
  
  
## Supplementary note
 - ***Be sure to cut off the power of the HC-08 module for any plug-in operation!!!***  
  
 - ***The HC-08 module must be set to the first-level energy-saving mode, otherwise the program cannot sleep, please ensure that the AT command is set successfully***  
  
  
  

