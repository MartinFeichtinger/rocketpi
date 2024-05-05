# pigpio
For all further C programms we will use the `pigpiod_if2.h` library to controll the GPIO-Pins of the Raspberry Pi.
This library allow to control the GPIOs via the socket interface to the pigpio daemon.

To be able to use this library and program the GPIO-Pins, the pigpio package must be installed as explained in the setup guild.

For more informations about pigpio: https://abyz.me.uk/rpi/pigpio/index.html

## GPIO-Pinout
![GPIO-Pinout-Diagram-2](https://github.com/MartinFeichtinger/rocketpi/assets/110301453/dc339ea9-93fa-431a-84f1-a48d85bf69fd)


## Test program
To test if the pigpio library is installed correctly you can use the `pigpioTest.c` programm.
This programm will toggle the GPIO 17 between HIGH and LOW. 
To make this visible you can use a led with a resistor connected to ground.

- build up the test schematic
- compile the test programm `gcc -Wall -pthread -o pigpioTest pigpioTest.c -lpigpiod_if2 -lrt`
- run the programm with `./pigpioTest`
