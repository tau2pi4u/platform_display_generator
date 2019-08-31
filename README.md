# platform_display_generator
## Requirements
The python script requires python 3.6 or newer.

Arduino Nano and a display compatible with the SSD1306 OLED Display library or an LCD display compatible with the Arduino LiquidCrystal library. 

## Use

### Configuring
To use, edit display config.json to contain the settings you want. Routes should end with spaces if you want there to be a gap when it wraps around. 

tScale is a multiplier for how fast the time on the board runs compared to real time.

tStart is the time displayed when the device boots.

lcdInputPins and oledInputPins are the pins used to select the displayed route for each setup. The first pin entered is least significant bit, the last is the most significant bit. They will be read as an integer, so if the array contains [2, 3, 4, 5] and the values on these pins are [1, 0, 1, 0], it'll be converted into the binary value 0101, or 5 in decimal (note that it's flipped). This would select route 5, which is Milton Keynes in the default config. Input 0 is reserved for a blank display, so there can be up to `2^n - 1` routes where n is the number of pins in the array (memory permitting).

"lcdControlPins" sets the pins for the lcd display control signals. The "data" pin list in "lcdControlPins" contains the data pins in the order [D0, D1, D2, D3]. Modify these to match how the display has been wired.

"oledControl" pins sets the pins for the oled control signals. For the OLED display, these shouldn't be modified, and the display should run off those pins.

"data" contains the various routes which can be displayed. The order here will be the index for the display, i.e. the 3rd one will require the input of 3 because of 0 being reserved for a blank display. To create a gap when the string loops around, add spaces at the end.

### Producing the .ino files

To produce the ino files for the arduino, run the script. It's recommended to do this by opening a terminal and running it from there in order to read the output. If there are errors at this stage, then the error message should explain the issue and these should be corrected. If a destination length is longer than the maximum length allowed by the display, it will be truncated and a warning will be given. 

### Compiling the code

If no errors occurred, there should now be an output folder, containing two folders, one for each of the display types. Load the required ino file into the arduino IDE (or your IDE of choice) and upload it. 

## Likely Issues

If the longest string is too long (~1000 characters for the OLED, 2000 for the LCD), then it may give a warning or error about memory and these will need to be made shorter. This is unlikely in normal use. 

If the json file contains errors, it will fail. Make sure to place commas in between list items, but not at the start or end of the list, e.g. `[1, 2, 3, 4]` is good, `[1, 2, 3, 4,]` is bad. The same goes for dictionaries, with the addition of a colon between the key and value, so `{"key" : "value", "key2" : "value2"}` is good, `{"key" : "value", "key2" : "value2",}` is bad. 

A pinlist for both displays is required, if only one is being used then just leave the other list as is.