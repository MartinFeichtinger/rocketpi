# RocketPi
The aim of this project is to controll a water rocket with a Raspberry Pi Zero.

## What is a water rocket?
A water rocket is a type of model rocket using water as its reaction mass.
The water is forced out by a pressurized gas, typically compressed air. Like all rocket engines, it operates on the principle of Newton's third law of motion.

<img width="960" height="1280" alt="image" src="https://github.com/user-attachments/assets/296fd27a-c8ff-4dd0-af4c-c687c98530f8" />

## What's special about our water rocket?
The fuel tank consists of a plastic bottle. This bottle can withstand a maximum pressure of 9 bar.
In order to achieve a higher flight altitude, we have decided to strengthen this plastic bottle with fiber-reinforced tape.
Whith this new tank we are able to reach pressures up to 13 bar.
Futhermore a Raspberry Pi Zero is used to eject the parachute and determine the flight altitude.
The parachute is attached to the top of the water rocket. To eject the parachute, the pre-tensioned handle is opened with three servo motors.
-image of the water rocket

## Rocket launcher
<img width="960" height="1280" alt="image" src="https://github.com/user-attachments/assets/5da3e651-4a59-48bb-96ba-272ecdfd00e5">

The rocket launcher had already been built beforehand. It also allows the rocket to be filled. Two ball valves can be used to switch between water and compressed air. The water rockets can then be launched by pulling a string.

## Rocketpi
<img src="https://github.com/MartinFeichtinger/rocketpi/assets/110301453/470a3fbe-7be8-42d9-b14c-0852b10e2277" width="600">

### Raspberry Pi Zero
The Raspberry Pi Zero is a tiny, affordable computer board developed by the Raspberry Pi Foundation. It's small but powerful, with features like a CPU, RAM, storage, and ports for connecting to displays and other devices. It's used for various projects like IoT, robotics, and gaming consoles due to its low cost and versatility.

### BerryIMUv4
IMU stands for Inertial Measurement Unit. It's a device that measures and reports specific forces like acceleration and angular velocity, using a combination of accelerometers, gyroscopes, and sometimes magnetometers. IMUs are commonly used in applications like navigation systems, robotics, virtual reality, and motion tracking. They provide essential data for determining an object's orientation, position, and movement in 3D space.

The BerryIMUv4 is an IMU that was specially developed for the rasberry pi zero. In addition to acceleration, rotation rate and magnetic field, it is possible to measure barometric altitude and temperature. Furthermore, there is a GPS module for position determination.

For more inforations visit: https://ozzmaker.com/product/berrygps-imu/

Example Programms: https://github.com/ozzmaker/BerryIMU

### Power Board
The power board connects the Raspberry Pi to the lithium battery and the servo motors. It also acts as a bridge between the Raspberry Pi and the BerryIMUv4 sensor module, and between the Raspberry Pi and the control panel.

#### Schematic
<img width="2824" height="1125" alt="Screenshot From 2026-02-27 09-51-35" src="https://github.com/user-attachments/assets/78cdb93a-9938-4a7f-b8a0-2616a460f10a" />

#### PCB layout
<img width="2970" height="1597" alt="Screenshot From 2026-02-27 09-52-22" src="https://github.com/user-attachments/assets/505f0639-f286-459c-b603-d91f75016ce3" />

### Controll Panel
The control panel enables interaction with the Raspberry Pi. On the one hand, control signals can be sent to the Raspberry Pi via a button, and on the other hand, the status of the Raspberry Pi can be displayed via LEDs. The entire system is switched on with a power switch. To signal that the Raspberry Pi is booting, a flashing signal is generated via a 555 timer. Immediately after the boot process, the reset input of the 555 timer is triggered and the boot LED stops flashing and remains lit. 

#### Schematic
<img width="2982" height="1795" alt="Screenshot From 2026-02-27 09-53-32" src="https://github.com/user-attachments/assets/29662c92-23fd-41b9-a8c5-887c7bc71d7e" />

#### PCB layout
<img width="3091" height="1795" alt="Screenshot From 2026-02-27 09-59-49" src="https://github.com/user-attachments/assets/160c1449-d1af-4f7d-80b7-bfb50a980644" />


## Software
### State-Machine
![stateMachine](https://github.com/MartinFeichtinger/rocketpi/assets/110301453/8d6aa8fb-2883-4f97-b338-c49b9545b5ed)

