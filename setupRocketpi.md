# Setup your Rocketpi
This projekt is developed for a **Raspberry Pi Zero 2W**. You can also use an Raspberry Pi Zero W but the *Remote Development Extenion* for VS-Code won't work.

## Prepare Raspberry Pi
- install the Raspberry Pi imager (`sudo apt install rpi-imager`)
- insert your microSD-Card (at least 8GB)
- open and choose **Raspberry Pi OS Lite (64-Bit)** for the **Raspberry Pi Zero 2W** (32-Bit  for Raspberry Pi Zero W)
- select your microSD-Card and write your image

## First Boot
- insert your microSD-Card in your Raspberry Zero 2W
- connect the Raspberry Pi to a monitor, a keyboard and powersupply and let him boot
- open the `raspi-config` and change hostname to `rocketpi-xx`  
- and activate `ssh`
- activate wlan (mobile HotSpot rescommended) and reboot
- (the order of this settings are important)

## Further configurations
- connect to the Raspi via ssh (`ssh pi@rocketpi-xx.local`) 
- open `raspi.config` and activate `i2c` and `Serial`
- close `raspi.config` and reboot

## Clone Rocketpi-Projekt
- install git: `sudo apt install git`
- `git clone https://github.com/MartinFeichtinger/rocketpi`
- bei Änderungen mit `git commit` und `git push` hochladen (username and token erforderlich)

## Configurations for the GPIO-Programming
- install the pigpio library `sudo apt install pigpio`
- setup the pigpio daemon to start at boot `sudo systemctl enable pigpiod`
- test the configuration with the `pigpioTest.c` program (located under pigpio) 

## Configurate VS-Code
- install vs-code on your ubuntu system
- install the **C/C++** extention
- install the **Remote Development** extention

## Programm Boot-LED
- `sudo nano /etc/rc.local`
- insert this two lines befor the `exit(0)`
- `/usr/bin/pigs m 27 w`
- `/usr/bin/pigs w 27 0`
