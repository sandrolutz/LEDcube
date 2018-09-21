Source code for LEDcube
=======================

This code is for board versions **1.***

## Setup

1. Install the Arduino IDE (See www.arduino.cc)
2. Copy the content of the folder ```bootloaders``` into
   - Windows:  ```<Arduino Installation>/hardware/arduino/avr/bootloaders/```
   - Mac OS X: ```/Applications/Arduino.app/Contents/java/hardware/arduino/avr/bootloaders/```
3. Copy the folder ```mega32``` to 
   - Windows:  ```<Arduino Installation>/hardware/arduino/avr/variants```
   - Mac OS X: ```/Applications/Arduino.app/Contents/java/hardware/arduino/avr/variants```
4. Append the following text to
   - Windows:  ```<Arduino Installation>/hardware/arduino/avr/boards.txt```
   - Mac OS X: ```/Applications/Arduino.app/Contents/java/hardware/arduino/avr/boards.txt```
   
   ```
   ledcube.name=LEDcube
   ledcube.build.mcu=atmega8
   ledcube.build.f_cpu=14745600L
   ledcube.build.core=arduino
   ledcube.upload.tool=avrdude
   ledcube.upload.protocol=arduino
   ledcube.upload.speed=19200
   ledcube.bootloader.tool=avrdude
   ledcube.bootloader.unlock_bits=0x3F
   ledcube.bootloader.lock_bits=0x0F
   
   ledcube.menu.cpu.atmega8=ATmega8 (4x4x4)
   ledcube.menu.cpu.atmega8.upload.maximum_size=7168
   ledcube.menu.cpu.atmega8.upload.maximum_data_size=1024
   ledcube.menu.cpu.atmega8.build.mcu=atmega8
   ledcube.menu.cpu.atmega8.build.variant=standard
   ledcube.menu.cpu.atmega8.build.board=X4
   ledcube.menu.cpu.atmega8.bootloader.low_fuses=0x3f
   ledcube.menu.cpu.atmega8.bootloader.high_fuses=0xca
   ledcube.menu.cpu.atmega8.bootloader.file=atmega8/ATmegaBOOT_14MHz.hex
   
   ledcube.menu.cpu.atmega32=ATmega32 (8x8x8)
   ledcube.menu.cpu.atmega32.upload.maximum_size=30720
   ledcube.menu.cpu.atmega32.upload.maximum_data_size=2048
   ledcube.menu.cpu.atmega32.build.mcu=atmega32
   ledcube.menu.cpu.atmega32.build.variant=mega32
   ledcube.menu.cpu.atmega32.build.board=X8
   ledcube.menu.cpu.atmega32.bootloader.low_fuses=0x3f
   ledcube.menu.cpu.atmega32.bootloader.high_fuses=0xca
   ledcube.menu.cpu.atmega32.bootloader.file=atmega/ATmegaBOOT_168_atmega32_14MHz.hex
   ```
5. Fix Serial library for ATmega32. Open
   - Windows:  ```<Arduino Installation>/hardware/arduino/avr/cores/arduino/HardwareSerial.cpp```
   - Mac OS X: ```/Applications/Arduino.app/Contents/java/hardware/arduino/avr/cores/arduino/HardwareSerial.cpp```
   
   Replace:
   ```
   #if defined(__AVR_ATmega8__)
      config |= 0x80; // select UCSRC register (shared with UBRRH) 
   #endif
   ```
   With:
   ```
   #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
      config |= 0x80; // select UCSRC register (shared with UBRRH) 
   #endif
   ```
   
## License
The MIT License (MIT)
  
  Copyright (c) 2016 Sandro Lutz
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
