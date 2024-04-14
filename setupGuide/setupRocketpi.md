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
- install git / github
- clone https://github.com/MartinFeichtinger/rocketpi

