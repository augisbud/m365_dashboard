# Xiaomi M365 Display 

<img src="https://user-images.githubusercontent.com/5514002/56957966-22138500-6b49-11e9-8e42-26d0758b6d00.jpg" width="200" height="264" />

# Products Used  
Arduino Pro Mini    
I2C OLED 0.96" or 1.3" Screen
FTD1232 Usb Programmer   
3d Printed Bracket  
1N4148 Diode  
0.25w 120ohm Resistor       

Estimated price is around 20$ (Inluding Printed Parts).

Knowing the price is around 20$ you can get the Xiaomi M365 Pro top panel from aliexpress for around the same price and a better look, unless you need additional features just go for the pro display.

# Flashing  
![alt text](https://i.imgur.com/DpPkvJz.jpg)  
Please install the libraries I provided in the files, install them to you arduino library folder, usually              
  C:\Users\\%username%\Documents\Arduino\libraries  
I'd recommend you to use Arduino 1.6.6  
https://www.arduino.cc/en/Main/OldSoftwareReleases  

# Physical Connections  
![alt text](https://camo.githubusercontent.com/a912641249173768ae60670e843c62294d06da4f/68747470733a2f2f656c656374726f2e636c75622f646174612f666f72756d2f6d657373616765732f34323633312f696d616765732f31313636302d313238302e6a7067)  

# Updating M365 firmware / Disabling the Dashboard
Turn on the scooter and immediately engage and hold the throttle and brake before the logo disappears from the dashboard LCD. You will enter on dashboard disabled mode.
The Arduino TX/RX pins will go to hi impedance state leaving the communication BUS free.

By this way you can update de M365 firmware without disconnecting the dashboard or any cable.

A new power cycle will reset the dashboard to normal mode.

# Known Issues  
Sometimes the Arduino Freezes, a watchdog is in place but doesn't always trigger.  

# Screen caps
# Soldering, soldered directly to the cable coming from the MCU
5V To Red    
GND To Black  
BUS To Yellow  
![alt text](https://i.imgur.com/3ZwcrIJ.jpg)  
A video on how everything is soldered may come soon.

Meanwhile you can enable subtitles in English in this YouTube video produced in Spanish language
https://www.youtube.com/watch?v=JQUNXCyj2Fs

# UI
UI pictures from version 0.2  
![alt text](https://i.imgur.com/8ekMdIo.jpg)  
![alt text](https://i.imgur.com/AHLVTcu.jpg)  

More pictures are coming soon.
