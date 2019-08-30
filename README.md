# platform_display_generator
Requires python3.6 or newer. 

To use, edit display config.json to contain the settings you want. Routes should end with spaces if you want there to be a gap when it wraps around. 

tScale is a multiplier for how fast the time on the board runs compared to real time.

tStart is the time displayed when the device boots.

lcdInputPins and oledInputPins are the pins for the displays.

lcdControlPins sets the pins for the lcd display control signals.

oledControl pins sets the pins for the oled control signals.

data contains the various routes which can be displayed. The order here will be the index for the display, i.e. the 3rd one will require the input of 3 (note: 0 is reserved for a blank display). 

The output folder contains two folders, each of which contains a .ino file which can be compiled and uploaded to an arduino.