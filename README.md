
# NDS version (orginal version of Constraint city)

You will need a working Nintendo DS compile chain (devkitpro). Watch the Makefile for further information.


# Android app (2nd generation corset)

## Software Android


```
cordova create projectname
cd projectname
cordova add platform android
```

adding some plugins

```
cordova plugin add org.apache.cordova.device
cordova plugin add cordova-plugin-network-information
cordova plugin add org.apache.cordova.device
cordova plugin add com.megster.cordova.bluetoothserial
```

running test on real device

android connected via USB and remote USB debugging enabled

`cordova run android`

and then launch chrome browser on desktop host machine 

`chrome://inspect/#`

select the app from list on android device; INSPECT!


## Hardware

### Robotis CM-700

`wget http://www.robotis.com/download/software/CM-9/ROBOTIS_OpenCM-v1.0.2-linux64.tgz`

### Robotis 9.04

Battery must be wired via +/- Vin pins (not the pins labelled with Li-Ion battery)

Dynamixel AX-12A motors connected through 3pin TTL interface

Bluetooth module connect with 4pins

Default settings: 1Mbps, Motor ID 1

`Dxl.begin(3); // set to 1Mbps baud rate` 

Change the motor ID by sending broadcast ID change (one motor at a time), check the firmware in DynamixelSetupBaudAndID/DXL_idchange 
