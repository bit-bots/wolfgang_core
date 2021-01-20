# Wolfgang CORE
COntrolling and REgulating electronics for the Wolfgang robot platform

Documentation can be found [here](http://doku.bit-bots.de/meta/manual/hardware/electronics/wolfgang_core.html).

## Features
* Power over Ethernet (PoE) power source (for ethernet camera)
* Power regulation 5V ~5A (odroid/raspberry pi), 9V 1A (network switch)
* Power source selection (Battery or power supply)
* Switching of power to Motors (Manual and through software)
* RS485/TTL on 4 Buses with theoretically up to 12 MBaud (4MBaud tested since thats what our Dynamixel motors support)
* Voltage (including Cell voltage for LiPos) and Current monitoring
* 3 RGB LEDS!!!

![Image of CORE](/core.png)
