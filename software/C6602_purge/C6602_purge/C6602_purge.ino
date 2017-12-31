//
// C6602_purge.ino - HP C6602 cartridge purge program
// 2017-12-30

#include <LiquidCrystal.h>

LiquidCrystal lcd(32, 31, 14, 30, 39, 15);

// Inkjet output
const int aux1 = 46;
const int aux2 = 47;
const int aux3 = 48;
const int aux4 = 49;
const int enable_pin = 10;

// Keypad pins (74HC165 based)
const int latchPin = 35;
const int dataPin = 34;
const int clockPin = 43;

// Input variables
boolean switchUp, switchDown, switchLeft, switchRight, switchEnter, switchMenu1, switchMenu2, switchMenu3;

// Others
boolean enabled;
boolean purge;
unsigned long nL_purged;

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
  
  enabled = false;
  purge = false;
  nL_purged = 0;
  
  // A little information
  lcd.begin(20, 4);
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0,0);
  lcd.print("C6602 purge program");
  lcd.setCursor(0,1);
  lcd.print("20V DISABLED");
  lcd.setCursor(0,2);
  lcd.print("Status: idle");
  lcd.setCursor(0,3);
  lcd.print("nL: 0");
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

  // Enable 20V output
  if(switchMenu1 && !purge)
    enabled = !enabled;

  // Check if we should be doing anything
  if(!enabled)  // If not
  {
    digitalWrite(enable_pin, LOW);  // Turn off 20V output
    lcd.setCursor(4,1);             // Update display
    lcd.print("DISABLED");
    lcd.setCursor(8,2);
    lcd.print("idle    ");
    delay(100);                     // Short delay to debounce
    return;                         // Reloop
  }
  else
  {
    digitalWrite(enable_pin, HIGH); // Turn on 20V output
    lcd.setCursor(4,1);             // Update display
    lcd.print("ENABLED ");
  }

  // Reset purge volume
  if (switchMenu2)
  {
    nL_purged = 0;
    lcd.setCursor(4,3);
    lcd.print("0               ");
  }
  
  // Purge on/off
  if (switchEnter && enabled)
  {
    purge = !purge;
  }
  
  if (purge)
  {
    // Pulse all nozzles 1000 times
    for (int i = 0; i < 1000; i++)
    {
      for (int j = 0; j < 12; j++)
      {
        pulseNozzle((j % 12) + 1);
      }
      delayMicroseconds(728);
    }
    nL_purged += 1920;              // 1920nL = 12000 drops * 160pL/drop
    lcd.setCursor(8,2);
    lcd.print("purging");
    lcd.setCursor(4,3);
    lcd.print(nL_purged, DEC);
    return;                         // Reloop
  }
  else
  {
    lcd.setCursor(8,2);
    lcd.print("ready   ");
    delay(400);
  }
  delay(100);                     // Short delay to debounce
}

void pulseNozzle (int n)
{
  PORTL |= n;             // Turn on nozzle
  delayMicroseconds(5);   // Pulse
  PORTL &= B11110000;     // Turn off
  delayMicroseconds(1);   // Safety
}

