# NabuKeys
Nabu Keyboard Tester/Translator/Emulator

This Arduino sketch can function in four different modes:

* Tester Mode: Capture keystrokes from a NABU keyboard and print them over serial.
* Forward Translator Mode: Use a NABU keyboard as a USB keyboard on a modern device.
* Emulator Mode: Read instructions over serial and send keystrokes to a NABU computer.
* Reverse Translator Mode: Use a modern USB keyboard as a keyboard on a NABU computer.

To compile the sketch you must add the USB Host Shield library to the Arduino IDE, found [here](https://github.com/felis/USB_Host_Shield_2.0). Or, if you do not plan to use Reverse Translator Mode, you can comment out the line `#define USBK_USE_HOST_SHIELD` in `usbkeysconfig.h`.

There are also gerber files for a shield. This is very useful as a few components are required to connect a NABU keyboard to an Arduino (or an Arduino to the keyboard port on a NABU computer).

## Components
For this sketch to work properly, you will need:

* 1x Arduino (duh)
* 1x DIN-6 connector
* 1x SN75176AP bus transceiver
* 1x 180Ω resistor
* 2x 1nF ceramic capacitor
* 1x IRLU110PBF or similar MOSFET
* 1x 10KΩ resistor

## Connections
![](wiki/schematic.png)

### NABU Keyboard Connections
* Pin 1, NC: not connected.
* Pin 2, GND: connect to MOSFET drain.
* Pin 3, 9V: connect to Arduino VIN. (DO NOT CONNECT TO +5V. The keyboard requires and the NABU provides 9V.)
* Pin 4, A: connect to SN75176AP pin 6 (A).
* Pin 5, B: connect to SN75176AP pin 7 (B).
* Pin 6, NC: not connected.

### Arduino Connections
* Digital pins 0-1: reserved for serial port.
* Digital pin 2: connect to MOSFET gate.
* Digital pin 3: connect to SN75176AP pin 1 (R).
* Digital pin 4: connect to SN75176AP pins 2 and 3 (#RE and DE).
* Digital pin 5: connect to SN75176AP pin 4 (D).
* Digital pin 6: not connected.
* Digital pins 7-13: reserved for USB Host Shield.
* Analog pins A0-A5: used to select settings.

### Other Connections
* MOSFET source to Arduino GND.
* 10KΩ resistor across MOSFET gate (equivalently digital pin 2) and Arduino GND.
* 180Ω resistor across SN75176AP pins 6 and 7 (equivalently DIN-6 pins 4 and 5).
* 1nF capacitor across SN75176AP pin 6 (equivalently DIN-6 pin 4) and Arduino GND.
* 1nF capacitor across SN75176AP pin 7 (equivalently DIN-6 pin 5) and Arduino GND.
* SN75176AP pin 5 to Arduino GND.
* SN75176AP pin 8 to Arduino +5V. (DO NOT CONNECT TO VIN.)

### "DIP Switch" Settings
Analog pins are used to select the mode of operation and settings for that mode. These are present on the shield as switches or jumpers labeled A-F. To leave the "DIP switch" in the OPEN position, leave the pin disconnected or set the switch to the UP position. To leave the "DIP switch" in the CLOSED position, connect the pin to GND or set the switch to the DOWN position.

![](wiki/switches.png)

* Analog pin 0 / switch A:
  * OPEN: `NK>` - Nabu Keyboard In. Tester / Forward Translator Mode.<br>Connect NABU keyboard to Arduino and Arduino to modern device.
  * CLOSED: `>NH` - To Nabu Host. Emulator / Reverse Translator Mode.<br>Connect modern device to Arduino and Arduino to NABU computer.
* Analog pin 1 / switch B:
  * OPEN: `SER` - Serial. Tester / Emulator Mode. Arduino communicates with a modern device over serial.
  * CLOSED: `USB`. Translator Mode. Arduino acts as a USB keyboard device or host, depending on switch A.
* Analog pin 2 / switch C:
  * TBD
* Analog pin 3 / switch D:
  * TBD
* Analog pin 4 / switch E:
  * TBD
* Analog pin 5 / switch F:
  * TBD

## Tester Mode
Connect a NABU keyboard to the Arduino. Connect the Arduino to a modern device over USB. Connect to the Arduino from the modern device over USB serial at 9600 baud (using the Arduino Serial Monitor, for example). Key events coming from the NABU keyboard will be printed over serial.

TBD

### Switch Settings
* Digital pins 5, 4 / switches A, B must both be OPEN in this mode.
* TBD

## Forward Translator Mode
To use Forward Translator Mode, you must first reflash the Arduino's USB controller with firmware that will allow the Arduino to function as a USB keyboard. Once you do this, you won't be able to program the Arduino until you reverse the process, so upload the sketch to the Arduino first.

### Reflashing the USB Controller Firmware
* Download the file `Arduino-keyboard-0.3.hex` in the `firmware` directory.
* Download and install ATMEL FLIP from https://www.microchip.com/developmenttools/ProductDetails/FLIP
* Connect the Arduino.
* Short the two male header pins on the Arduino closest to the USB port for a few seconds. (If you're on Windows, you should hear the "device disconnected" sound when connecting the pins and the "device connected" sound when disconnecting the pins.)

![](wiki/dfureset.jpg)

* Open ATMEL FLIP.
* Click the button for "Select a Target Device" and select ATmega16U2.
* Click the button for "Select a Communication Medium" and select USB, then click Open.
* Go to Buffer > Options. Make sure "Reset Buffer Before Loading" is set to Yes and "Address Programming Range" is set to Whole Buffer.
* If this is your first time doing this:
  * Click the button for "Read Target Device Memory".
  * Click the button for "Save Buffer As HEX File" and save it as, for example, `arduino.hex`. (You can use this file later to turn your Arduino from a keyboard back into an Arduino.)
* Click the button for "Load HEX File" and select the `Arduino-keyboard-0.3.hex` file.
* Click the Run button.
* Once the programming is done, disconnect the Arduino.

To reverse this process, do the same thing but select your saved `arduino.hex` file instead of the `Arduino-keyboard-0.3.hex` file.

### Using Forward Translator Mode
Connect a NABU keyboard to the Arduino. Connect the Arduino to a modern device over USB. Use the NABU keyboard as a USB keyboard.

TBD

### Switch Settings
* Digital pin 5 / switch A must be OPEN.
* Digital pin 4 / switch B must be CLOSED.
* TBD

## Emulator Mode
Connect a modern device to the Arduino over USB. Connect the Arduino to the NABU keyboard port. Connect to the Arduino from the modern device over USB serial at 9600 baud (using the Arduino Serial Monitor, for example).

TBD

### Switch Settings
* Digital pin 5 / switch A must be CLOSED.
* Digital pin 4 / switch B must be OPEN.
* TBD

## Reverse Translator Mode
To use Reverse Translator Mode, you must add a USB Host Shield to the Arduino. [Sparkfun has one for sale.](https://www.sparkfun.com/products/9947) You must also add the USB Host Shield library to the Arduino IDE, found [here](https://github.com/felis/USB_Host_Shield_2.0).

If using the Sparkfun USB Host Shield, you'll need to add a wire from digital pin 7 to the RESET line. Additionally, if you are using an Arduino Mega, you'll need to route pins 13, 12, 11, and 10 to pins 52, 50, 51, and 53, respectively.

Connect a USB keyboard to the USB Host Shield. Connect the Arduino to the NABU keyboard port. Use the USB keyboard as the NABU's keyboard.

TBD

### Switch Settings
* Digital pins 5, 4 / switches A, B must be CLOSED.
* TBD

## Acknowledgements

Special thanks to: [Jason Thorpe](https://github.com/thorpej) for [nabu-keyboard-usb](https://github.com/thorpej/nabu-keyboard-usb)
