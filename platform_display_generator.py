import json
import os
import shutil

maxLen = 21
maxRoutes = int(0xFFFFFFFF)
outputDir = "output"
lcdInoName= "lcd_platform_display"
oledInoName= "oled_platform_display"
lcdDir = os.path.join(outputDir, lcdInoName)
oledDir = os.path.join(outputDir, oledInoName)
configWarningCount = 0

# Inputs are a list of requested pins and a list of pins which are already used
def ValidatePins(input, blocked, maxRoutes):
	inputPins = [-1] * len(input)
	rejectedPins = []
	for i, pin in enumerate(input):
		if(pin not in blocked):
			inputPins[i] = pin
		else:
			rejectedPins.append(i)
	blocked.append(inputPins)
	for i in rejectedPins:
		if(inputPins[i] != -1):
			continue
		for tryPin in range(2, 22):
			if(tryPin not in inputPins and tryPin not in blocked):
				inputPins[i] = tryPin
		if(inputPins[i] == -1):
			print("Failed to find a valid input pin")
			exit(1)
	pinMax = (1 << len(inputPins)) - 2
	maxRoutes = min(maxRoutes, pinMax)
	return inputPins, maxRoutes
	
def ValidateDestinations(data):
	global configWarningCount
	destinations = []
	destStrLength = 0
	for item in data:
		newDest = item["destination"][0:maxLen]
		if(len(item["destination"]) > maxLen):
			print(f"Shortening the length of destination {item['destination']} to {newDest}")
			configWarningCount += 1
		destinations.append(newDest)
		destStrLength = max(destStrLength, len(newDest))
	return {"dests": destinations, "destStrLen": destStrLength + 1}
	
def ValidateRoutes(data):
	routes = []
	routeStrLength = 0
	for item in data:
		newRoute = item["route"]
		routes.append(newRoute)
		routeStrLength = max(routeStrLength, len(newRoute))
	if(len(routes) > maxRoutes):
		print("Too many routes")
		exit(1)
	return {"routes": routes, "routeStrLen": routeStrLength + 1}

# Load the config from the provided json
try:
	with open("display_config.json", "r") as inCfg:
		config = json.load(inCfg)
except:
	print("Failed to load the file \'display_config.json\', please check that it exists and has valid formatting")
	exit(-1)

# Pins used by the lcd display for data transfer
lcdUsedPins = []
lcdUsedPins.extend(config['lcdControlPins']['data'])
lcdUsedPins.append(config['lcdControlPins']['rs'])
lcdUsedPins.append(config['lcdControlPins']['en'])

# Pins used by the OLED for data transfer
oledUsedPins = config['oledControlPins']

lcdInputPins, maxRoutes = ValidatePins(config['lcdInputPins'], lcdUsedPins, maxRoutes)
oledInputPins, maxRoutes = ValidatePins(config['oledInputPins'], oledUsedPins, maxRoutes)
numPins = len(lcdInputPins)
if(numPins != len(oledInputPins)):
	print(f"{numPins} pins given for lcd pins but {len(oledInputPins)} given for oled - please fix this")
	exit(1)
maxInput = (1 << numPins) - 1

destInfo = ValidateDestinations(config['data'])
routeInfo = ValidateRoutes(config['data'])


print(f"lcdInputPins: {lcdInputPins}")
print(f"oledInputPins: {oledInputPins}")
print(f"Destinations: {destInfo}")
print(f"Routes: {routeInfo}")
if(configWarningCount == 0):
	print("Successfully loaded and parsed config.")
else:
	print(f"Loaded and parsed config with {configWarningCount} warning{'s' if configWarningCount > 1 else ''}.")

try:
	if not os.path.exists(outputDir):
		os.makedirs(outputDir)

	if not os.path.exists(oledDir):
		os.makedirs(oledDir)
	oledPath = os.path.join(oledDir, "display.hpp")
	with open(oledPath, "w") as oledHeader:
		oledHeader.write(f"/*************************\nAUTO GENERATED\n*************************/\n\n")
		oledHeader.write("#include <Adafruit_GFX.h>\n")
		oledHeader.write("#include <Adafruit_SSD1306.h>\n")
		oledHeader.write("#define OLED_DISPLAY\n")
		oledHeader.write(f"const unsigned int pinArray[{numPins}] = ")
		oledHeader.write("{ ")
		first = True
		for pin in oledInputPins:
			if not first:
				oledHeader.write(', ')
			oledHeader.write(f"{pin}")
			first = False
		oledHeader.write("};\n")
		oledHeader.close()

	if not os.path.exists(lcdDir):
		os.makedirs(lcdDir)
	lcdPath = os.path.join(lcdDir, "display.hpp")
	with open(lcdPath, "w") as lcdHeader:
		lcdHeader.write(f"/*************************\nAUTO GENERATED\n*************************/\n\n")
		lcdHeader.write("#include <LiquidCrystal.h>\n")
		lcdHeader.write("#define LCD_DISPLAY\n")
		lcdHeader.write(f"#define RS {config['lcdControlPins']['rs']}\n")
		lcdHeader.write(f"#define EN {config['lcdControlPins']['en']}\n")
		for i, pin in enumerate(config['lcdControlPins']['data']):
			lcdHeader.write(f"#define D{i+4} {pin}\n")
		lcdHeader.write(f"const unsigned int pinArray[{numPins}] = ")

		lcdHeader.write("{ ")
		first = True
		for pin in oledInputPins:
			if not first:
				lcdHeader.write(', ')
			lcdHeader.write(f"{pin}")
			first = False
		lcdHeader.write("};\n")
		lcdHeader.close()

	with open("output/common.hpp", "w") as common:
		common.write(f"/*************************\nAUTO GENERATED\n*************************/\n\n")
		common.write(f"#define MAX_LEN {maxLen}\n")
		common.write(f"#define NUM_STRINGS {len(destInfo['dests']) + 1}\n")
		common.write(f"#define ROUTE_BUF_SIZE {routeInfo['routeStrLen']}\n")
		common.write(f"#define DEST_BUF_SIZE {destInfo['destStrLen']}\n")
		common.write(f"#define T_SCALE {config['tScale']}\n")
		common.write(f"#define START_HOUR {config['tStart']}\n")
		common.write(f"#define NUM_PINS {numPins}\n")
		common.write(f"#define MAX_INPUT {maxInput}\n")
		
		#dest strings
		common.write("\n/* DEST STRINGS */\n\n")
		i=0
		common.write(f"const char dest{i}[] PROGMEM = \"\\0\";\n")
		for dest in destInfo["dests"]:
			i+=1
			common.write(f"const char dest{i}[] PROGMEM = \"{dest}\";\n")
		
		common.write("const char * const destArray[NUM_STRINGS] PROGMEM= {\n")
		for index in range(0, len(destInfo['dests']) + 1):
			common.write(f"\tdest{index},\n")
		common.write("};\n")
		
		#route strings
		common.write("\n/* ROUTE STRINGS */\n\n")
		i=0
		common.write(f"const char route{i}[] PROGMEM = \"\\0\";\n")
		for route in routeInfo['routes']:
			i+=1
			common.write(f"const char route{i}[] PROGMEM = \"{route}\";\n")
		
		common.write("const char * const routeArray[NUM_STRINGS] PROGMEM = {\n")
		for index in range(0, len(routeInfo['routes']) + 1):
			common.write(f"\troute{index},\n")
		common.write("};\n")
		common.close()

	inoName = lcdInoName + ".ino"
	lcdMainPath = os.path.join(lcdDir, inoName)
	shutil.copy2("main.cpp", lcdMainPath)

	commonPath = os.path.join(outputDir, "common.hpp")
	commonCpyPath = os.path.join(lcdDir, "common.hpp")
	shutil.copy2(commonPath, commonCpyPath)
	commonCpyPath = os.path.join(oledDir, "common.hpp")
	shutil.copy2(commonPath, commonCpyPath)

	inoName = oledInoName + ".ino"
	oledMainPath = os.path.join(oledDir, inoName)
	shutil.copy("main.cpp", oledMainPath)
	print("Successfully produced arduino source code.")
except Exception as e:
	print("Failed to produce Arduino source code with reason {e}.")
	


	
