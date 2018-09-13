#include "display.hpp"
#include "common.hpp"

void printInfo(unsigned short info, unsigned long lastUpdateTime);
void clearDestRow();
void clearRouteRow();
void clearTimeRow();
void clearInfoRow();
void clearDisplay(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void setDestCursor();
void setRouteCursor();
void setTimeCursor();
void setInfoCursor();
void setCursor(unsigned int x, unsigned int y);
void print(String s);
void print(char c);
void print(int i);
void print(unsigned int i);
void print(const char * s);
void print(unsigned long i);
void print(unsigned short i);
void display();
void initDisplay();
void clearDisplay();
void setTextColour(unsigned int colour);
void setTextSize(unsigned int size);

#ifdef OLED_DISPLAY
Adafruit_SSD1306 disp(-1);

void clearDestRow()
{
	clearDisplay(0, 0, 128, 8);
}

void clearRouteRow()
{
	clearDisplay(0, 12, 128, 8);
}

void clearTimeRow()
{
	clearDisplay(0, 24, 128, 32);
}

void clearInfoRow()
{
	//do nothing
}

void clearDisplay(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) 
{
  disp.writeFillRect(x1, y1, x2, y2, BLACK);
}

void setDestCursor()
{
	setCursor(0,0);
}
void setRouteCursor()
{
	setCursor(0,12);
}
void setTimeCursor()
{
	setCursor(40,24);
}

void setInfoCursor()
{
	// do nothing
}

void setCursor(unsigned int x, unsigned int y)
{
  disp.setCursor(x,y);
}

void printInfo(unsigned short info, unsigned long lastUpdateTime)
{
	// do nothing
}

void print(String s)
{
  disp.print(s);
}

void print(unsigned short i)
{
	disp.print(i);
}

void print(const char * s)
{
  disp.print(s);
}

void print(char c)
{
  disp.print(c);
}

void print(int i)
{
  disp.print(i);
}
void print(unsigned long i)
{
	disp.print(i);
}
void print(unsigned int i)
{
  disp.print(i);
}

void display()
{
  disp.display();
}

void initDisplay()
{
  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void clearDisplay()
{
  disp.clearDisplay();
}

void setTextColour(unsigned int colour)
{
  disp.setTextColor(colour);
}

void setTextSize(unsigned int size)
{
  disp.setTextSize(size);
}

#else
#define WHITE 0 // dummy 
LiquidCrystal disp(RS, EN, D4, D5, D6, D7);
void clearTimeRow()
{
  clearDisplay(0, 2, 19, 2);
}
void clearDestRow()
{
  clearDisplay(0, 0, 19, 0);
}
void clearRouteRow()
{
  clearDisplay(0, 1, 19, 1);
}

void clearInfoRow()
{
	clearDisplay(0, 3, 19, 3);
}

void clearDisplay(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  if(x2 > x1)
  {
    unsigned int x = x1;
    x1 = x2;
    x2 = x;
  }
  if(y1 > y2)
  {
    unsigned int y = y1;
    y1 = y2;
    y2 = y;
  }
  for(unsigned int y = y1; y <= y2; y++)
  {
    setCursor(x1, y);
    for(unsigned int x = x1; x <= x2; x++)
    {
      print(' ');
    }
  }
}

void setDestCursor()
{
	setCursor(0,0);
}
void setRouteCursor()
{
	setCursor(0,1);
}
void setTimeCursor()
{
	setCursor(0,2);
}

void setInfoCursor()
{
	setCursor(0,3);
}

void setCursor(unsigned int x, unsigned int y)
{
  disp.setCursor(x, y);
}

void printInfo(unsigned short info, unsigned long lastUpdateTime)
{
	
	clearInfoRow();
	setInfoCursor();
	print("In:");
	if(info > NUM_STRINGS)
	{
		print(info);
	}
	else
	{
		print("N/A");
	}
	setCursor(10, 3);
	print(" at:");
	print(lastUpdateTime);
}

void print(String s)
{
  disp.print(s);
}
void print(char c)
{
  disp.print(c);
}
void print(int i)
{
  disp.print(i);
}

void print(unsigned short i)
{
	disp.print(i);
}

void print(unsigned int i)
{
  disp.print(i);
}
void print(unsigned long i)
{
	disp.print(i);
}
void print(const char * s)
{
  disp.print(s);
}
void display()
{
  //do nothing
}
void initDisplay()
{
  disp.begin(20, 4);
}
void clearDisplay()
{
  disp.clear();
}
void setTextColour(unsigned int colour)
{
  //do nothing
}
void setTextSize(unsigned int size)
{
  //do nothing
}
  
#endif

typedef struct _TIME
{
  unsigned short hr;
  unsigned short mn;
  unsigned short sc;
} TIME;

TIME t, nextTrainTime;

void calculateTime()
{
  unsigned long secs = millis()*T_SCALE/1000;
  secs %= 86400;
  t.hr = (START_HOUR + (secs/3600)) % 24;
  t.mn = (secs/60) % 60;
  t.sc = secs % 60;
}

void printTime()
{
  char printChar = (t.hr / 10) + '0';
  print(printChar);
  printChar = (t.hr % 10) + '0';
  print(printChar);
  print(':');
  printChar = (t.mn / 10) + '0';
  print(printChar);
  printChar = (t.mn % 10) + '0';
  print(printChar);
  print((char) ':');
  printChar = (t.sc / 10) + '0';
  print(printChar);
  printChar = (t.sc % 10) + '0';
  print(printChar);
}

void calculateNextTrainTime()
{
  nextTrainTime.hr = t.hr;
  for(unsigned short i = 5; i < 60; i+=5)
  {
    if(i > t.mn + 4)
    {
      nextTrainTime.mn = i;
      return;
    }
  }
  nextTrainTime.hr++;
  nextTrainTime.mn = 0;
  nextTrainTime.hr %= 24;
}

bool isDelayed()
{
  if(nextTrainTime.hr < t.hr)
    return true;
  if(nextTrainTime.hr == t.hr && nextTrainTime.mn < t.mn)
    return true;
  if(nextTrainTime.hr == t.hr && nextTrainTime.mn == t.mn && nextTrainTime.sc < t.sc)
    return true;
  return false;
}

void printNextTrainTime()
{
  char printChar = (nextTrainTime.hr / 10) + '0';
  print(printChar);
  printChar = (nextTrainTime.hr % 10) + '0';
  print(printChar);
  print((char) ':');
  printChar = (nextTrainTime.mn / 10) + '0';
  print(printChar);
  printChar = (nextTrainTime.mn % 10) + '0';
  print(printChar);
}


char nullReplacedChar = '\0';
unsigned int currNullPos;
unsigned int currStartPos;
char route[ROUTE_BUF_SIZE];
char dest[DEST_BUF_SIZE];
unsigned int currString;
unsigned int currStrLen;

unsigned long long nextRotate = 0;

void initString(unsigned int i)
{
  currString = i;
  strcpy_P(route, (char*) pgm_read_word(&(routeArray[i])));
  currStrLen = strlen(route) + 1;
  currNullPos = MAX_LEN > currStrLen ? currStrLen : MAX_LEN;
  nullReplacedChar = route[currNullPos];
  route[currNullPos] = '\0';
  currStartPos = 0;
}

void rotateString()
{
  if (currStrLen + 1 < MAX_LEN)
  {
    return;
  }
  route[currNullPos] = nullReplacedChar;
  currNullPos++;
  currNullPos %= currStrLen;
  currStartPos++;
  currStartPos %= currStrLen;
  nullReplacedChar = route[currNullPos];
  route[currNullPos] = '\0';
}

void printString()
{
  clearRouteRow();
  setRouteCursor();
  if (currStrLen < MAX_LEN)
  {
    print(route);
    return;
  }
  char * start = &route[0] + currStartPos;
  print(start);
  if ((int) currStartPos > (int) currStrLen - MAX_LEN)
  {
    print(route);
  }
}

void GetNewDestString(unsigned int currString)
{
	strcpy_P(dest, (char*) pgm_read_word(&(destArray[currString])));
	clearDestRow();
	setDestCursor();      // begin text at this location 
	calculateNextTrainTime();
    printNextTrainTime();
    print(" ");
    print(dest);
}

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  for(unsigned short i = 1; i < NUM_PINS; i++)
  {
    pinMode(pinArray[i], INPUT);
  }
  initDisplay();
  clearDisplay();
  setTextColour(WHITE); // or BLACK);
  setTextSize(1);      // printable sizes from 1 to 8; typical use is 1, 2 or 4
  setDestCursor();      // begin text at this location 
  GetNewDestString(0);
  setTimeCursor();
  calculateTime();
  printTime();
  display();
  initString(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned short currentDisplayStr = 0;
  unsigned long lastUpdateTime = 0;
  unsigned short lastString = currString;
  currString = 0;
  for(unsigned short i = 0; i < NUM_PINS; i++)
  {
    if(digitalRead(pinArray[i]))
    {
      currString += 1 << i;
    }
  }
  currString = currString < NUM_STRINGS ? currString : 0;
  
  if(currString != lastString && currString < NUM_STRINGS && currString != 0)
  {
	lastUpdateTime = millis()/1000;
	currentDisplayStr = currString;
	GetNewDestString(currString);
	initString(currString);
    printString();
    display();
    nextRotate = millis() + 1500;
  }
  else if(currString == MAX_INPUT)
  {
	lastUpdateTime = millis()/1000;
	currentDisplayStr = 0;  
	GetNewDestString(currString);
	initString(0);
	printString();
  }
  
  // time
  clearTimeRow();
  setTimeCursor();
  calculateTime();
  printTime();

  // info
  if(millis() > nextRotate)
  { 
  rotateString();
  printString();
  nextRotate += 250;
  }
  printInfo(currentDisplayStr, lastUpdateTime);
  display();
} 