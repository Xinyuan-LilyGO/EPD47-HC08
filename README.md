<h1 align = "center">🌟 LilyGo-EPD47-HC08-PDM_MIC-Test 🌟</h1> 

## **English | [中文](./README_CN.md)**  
  
## Introduction
### 1.This is a Dome about testing the HC08-PDM_MIC low energy Bluetooth module. 
   -  Install the HC08-PDM_MIC module on a LilyGo-EPD47 screen and put it to sleep  
     
   -  Use another piece of ESP32 to wake up the sleeping LilyGo-EPD47 at any time, send the character text that needs to be displayed, and make it enter the microphone for detection  
     
   -  When the sound is detected, it will go to sleep again. Press the defined button to repeat the above test steps.  
  
  
## Hardware preparation
1.HC08-PDM_MIC Bluetooth module  
  
2.USB to TTL serial port module  
  
3.LilyGo-EPD47  
  
4.ESP32 Development board  
  
5.Button  
  
6.FPC 6 Pin  
  
![Hardware display](/images/1.jpg)  
  
  
  
## HC08 AT设置
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

![HC08install](/images/2.jpg)
**(warning: The power must be turned off, otherwise the ESP32 module of the screen may be burned out)**  
  
  
  
## Upload firmware
### 1.LilyGo-EPD47 firmware  
 -  Open with ArduinoIED \examples\epd47_show\epd47_show.ino  
   
 -  **Please select the development board"TTGO T-Watch"**(Otherwise, the compilation will fail because the project occupies too much storage)  
   
 -  After selecting the correct port, you can compile and upload. After the upload is successful, wait for a while until the screen displays the following text, which means success  
  
![EPD47show1](/images/4.jpg)  
"sleep began, wait for wake up"  
  
  
  
  
### 2. EPS32 Development board firmware  
 -  Open with ArduinoIED  \examples\epd47_blue_waker_center\epd47_blue_waker_center.ino  
  
 -  **Please confirm your button pin before compiling**，The button pin is 15  
  
 -  After selecting the correct port, you can compile and upload. After the upload is successful, wait for about 10 seconds,Until the screen splash screen displays the following text  
  
![EPD47show2](/images/5.jpg) 
"Please blow into the microphone"  
  
  
  
  
## Test steps
### 1. When the screen displays：“请对麦克风吹气”,Please blow into the microphone
  
![PDM_MIC](/images/6.jpg) 
  
  
  
### 2.When the microphone detects air pressure, ESP32 enters the sleep state, waiting to be activated again  
  
![EPD47show3](/images/7.jpg) 
"microphone test normally, sleep began"  
  
  
  
### 3.If you want to repeat the test again, you can press the button, wait for the ink screen (about 5 seconds) to flash and display the text, then repeat the test steps  
  
![ButtonPress](/images/8.jpg) 
  
  
  
  
## Demo video
https://github.com/Cai1902126141/LilyGo-EPD47/blob/main/DemoVideo.mp4?raw=true  
  
  
## Supplementary note
 - ***Be sure to cut off the power of the HC-08 module for any plug-in operation!!!***  
  
 - ***The HC-08 module must be set to the first-level energy-saving mode, otherwise the program cannot sleep, please ensure that the AT command is set successfully***  
  
 - Before testing, please make sure that the HC-08 microphone module and EPD47 are connected securely  
  
 - If the screen does not respond after pressing the button, try to press the button vigorously for a longer time to ensure that the button is triggered  
  
 - After pressing the button to wake up the screen, you must first complete the microphone test steps, otherwise it will wait for the microphone input and will not sleep  
  
  

