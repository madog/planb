//
// C6602_test.ino - HP C6602 test program
// 2017-10-12

#include <LiquidCrystal.h>

LiquidCrystal lcd(32, 31, 14, 30, 39, 15);

// Inkjet output
const int aux1 = 46;
const int aux2 = 47;
const int aux3 = 48;
const int aux4 = 49;
const int enable_pin = 2;

// Keypad pins
// Pin connected to latch pin (  ) of 74HC165
const int latchPin = 35;
// Pin connected to data pin (CP) of 74HC165
const int dataPin = 34;
// Pin connected to clock pin (  ) of 74HC165
const int clockPin = 43;

// Input variables
boolean bitValue;
boolean switchUp, switchDown, switchLeft, switchRight, switchEnter, switchMenu1, switchMenu2, switchMenu3;

// Others
byte nNum;
boolean nEnabled;

void setup()
{
  // Keypad
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);
  
  // Inkjet output
  pinMode(aux1, OUTPUT);
  pinMode(aux2, OUTPUT);
  pinMode(aux3, OUTPUT);
  pinMode(aux4, OUTPUT);

  // Direct port manipulation for inkjet (PORTL)
  DDRL = DDRL | B1111;
  // Turn all off
  PORTL &= B11110000;
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  
  nNum = 0;
  nEnabled = FALSE;
  
  // A little information
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("C6602 test program");
  lcd.setCursor(1,0);
  lcd.print("Nozzle: None");
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
  
  if()
}