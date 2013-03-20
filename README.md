Arduino
=======

This is all the Arduino code for the book in a single repository, unlike the Java code, the Arduino one is very compact, and it would make little sense to separate it in different repositories. It also includes the libraries used for the projects.

# Materials

There are a series of examples, mostly the full listings for the book plus a couple of debug tools we used while writing it. On top of that there are three libraries: the USB Host library, based on the one published on the Arduino website; the P2PMQTT one, our own way to get a minimal communication protocol to send data between devices in a broker free infrastructure; and a library to control long lines of LEDs strongly based on one by Adafruit Industries (thanks Limor!).

### Author/s

This code was written by Andreas G�ransson and David Cuartielles for a book called Android Open Accessory Programming with Arduino.

## Getting the source...

1. **Clone** this repository to your development machine
2. Unzip it in your Arduino sketchbook, so that your libraries will end up at the right level
3. **Boot the IDE** and browse the code inside your **sketchbook** that should now be including all the examples

### If there are compilation errors

4. Check the error and send us a report on GitHub, if you debugged it yourself, please send us a pull request
5. Be patient, we will put our brain power at the service of your curiosity!

## This code is compatible with Arduino's IDE v1.0 and newer

### Adding P2PMQTT to your Project

1. **#include "P2PMQTT.h"** the library and make the calls to cofigure the accessory mode

