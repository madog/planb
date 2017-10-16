//
// pump_test.ion - 12V pump test program
// 2017-10-16

#include <LiquidCrystal.h>

LiquidCrystal lcd(32, 31, 14, 30, 39, 15);

// Output pin
const int pin_pump = 2;
// Keypad pins (74HC165 based)
const int latchPin = 35;
const int dataPin = 34;
const int clockPin = 43;

// Variables
// Inputs
boolean switchUp, switchDown, switchLeft, switchRight, switchEnter, switchMenu1, switchMenu2, switchMenu3;

// Time to run pump in milliseconds
unsigned int time_pump = 1000;

void setup()
{
  // Set up pins
  pinMode(pin_pump, OUTPUT);
  digitalWrite(pin_pump, LOW);
  // Keypad
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);

  // Set up LCD
  lcd.begin(20, 4);
  lcd.clear();
  // A little information
  lcd.noCursor();
  lcd.setCursor(0,0);
  lcd.print("12V test program");
  lcd.setCursor(0,1);
  lcd.print("Pump: STOPPED");
  lcd.setCursor(0,2);
  lcd.print("Time: 00.000s");
}

void loop()
{
  // Latch keypad shift register
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
  
  // Read each value individually
  for (byte n = 0; n < 8; n++)
  {
   switch (n)
    {
      case 0:
      switchMenu3 = !(digitalRead(dataPin));
      break;
      case 1:
      switchMenu2 = !(digitalRead(dataPin));
      break;
      case 2:
      switchMenu1 = !(digitalRead(dataPin));
      break;
      case 3:
      switchUp    = !(digitalRead(dataPin));
      break;
      case 4:
      switchRight = !(digitalRead(dataPin));
      break;
      case 5:
      switchEnter = !(digitalRead(dataPin));
      break;
      case 6:
      switchDown  = !(digitalRead(dataPin));
      break;
      case 7:
      switchLeft  = !(digitalRead(dataPin));
      break;
    }
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }

  // Run pump if requested
  if (switchEnter)
  {
    lcd.setCursor(6,1);
    lcd.print("RUNNING");
    digitalWrite(pin_pump,HIGH);
    delay(time_pump);
    digitalWrite(pin_pump,LOW);
    lcd.setCursor(6,1);
    lcd.print("STOPPED");
  }
  
  // Adjust time if requested
  if (switchUp && time_pump < 99999)
  {
    if (switchMenu1)
      time_pump += 100;
    else if (switchMenu2)
      time_pump += 10;
    else if (switchMenu3)
      time_pump += 1;
    else
      time_pump += 1000;
  }
  if (switchDown && time_pump > 0)
  {
    if (switchMenu1)
      time_pump -= 100;
    else if (switchMenu2)
      time_pump -= 10;
    else if (switchMenu3)
      time_pump -= 1;
    else
      time_pump -= 1000;
  }

  // Update time display
  lcd.setCursor(6,2);
  // Zero padding
  if (time_pump < 10)
    lcd.print("0000");
  else if (time_pump < 100)
    lcd.print("000");
  else if (time_pump < 1000)
    lcd.print("00");
  else if (time_pump < 10000)
    lcd.print("0");
  lcd.print(time_pump,DEC);
  
  // Short delay before reloop
  delay(100);
}
