/* 
 Firmware writen by Yvo de Haas.
 Latest version (V0.05) written 12 April 2014.
 This firmware was specifically writen for Plan B. Operation on any other printer can not be guaranteed.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 For the GNU General Public License, see <http://www.gnu.org/licenses/>.
 */

/*
 The main tab is for all overhead, all values, definitions, variables and settings. The program itself (between the for loop() brackets) only consists of a few procedures. 
 Everything else is specified over all other tabs. 
 
 
 Hardware:
 * 74HC165 shift input register atached to pins 35, 34 and 43 of arduino, as detailed below.
 * the encoder also attached on the keypad is attached to pins 45 and 44. The printer should be able to function without encoder.
 * HD44780 20x4 LCD attached to pins 32, 31, 14, 30, 39 and 15 of arduino.
 * SD chipselect is attached to arduino pin 53.
 * X motor (X driver) step, dir and enable are connected to pins A4, A3 and A5.
 * Y motor (Y driver) step, dir and enable are connected to pins 5, 17 and 4.
 * S motor (Z driver) step, dir and enable are connected to pins 16, 11 and 3.
 * F1 motor (E0 driver) step, dir and enable are connected to pins 28, 27 and 29.
 * F2 motor (E1 driver) step, dir and enable are connected to pins 25, 24 and 26.
 * B motor (E2 driver) step, dir and enable are connected to pins 22, A6 and 23.
 * X endstop (X min) is connected to pin 37.
 * Y endstop (X max) is connected to pin 40.
 * S endstop (Y min) is connected to pin 41.
 * F1 endstop (Y max) is connected to pin 38.
 * F2 endstop (Z min) is connected to pin 18.
 * B endstop (Z max) is connected to pin 19.
 * buzzer attached to pin A7.
 * nozzles attached to pins 46, 47, 48 and 49;
 * nozzle power safety (extruder 0) is attached to pin 2; 
 * Heated bed sensor attached to pin A15
 * Heated bed heater attached to pin 10
 
 */

#include <LiquidCrystal.h>
#include <SD.h>


//configurations <--------------------------------------------------------------------------------------------------------------------------------------------
//movement speeds
const int X_print_speed = 60;
const int travel_speed = 60;
const int single_nozzle_print_speed = 30;

const int acc_max_delay_time = 5000; //in microseconds
const int constant_movement_X_acceleration = 1000; //in mm/s^2

//estimate variables
const int estimate_new_layer_time = 12000; // in milliseconds
const int estimate_speed = 50; //in mm/s.

//cartridge resolution
const int printing_DPI = 96;

//Printing configurations
//the offset in amount of steps the nozzle starts earlier than the target to compensate for the movement speed in both directions.
//if the lines start too early in relation to eachother, increase the value, if they start too late, decrease the value.
//needs to be reconfigured every time the printing speed gets increased.
const byte printing_double_direction_offset = 2; //in steps (at 20 steps/mm this means 0.05mm per step.)
const byte printing_single_nozzle_print_nozzle = 0; // the nozzle (0-11) that needs to be triggered when the printer is printing single nozzle from Gcode.



const int chipSelect = 53;

File printFile;

//pin names
LiquidCrystal lcd(32, 31, 14, 30, 39, 15);

//input shift register
// Pin connected to latch pin (  ) of 74HB165
const int latchPin = 35;
// Pin connected to data pin (CP) of 74HC165
const int dataPin = 34;
// Pin connected to clock pin (  ) of 74HC165
const int clockPin = 43;

//input encoder
const int encoderPin1 = 45;
const int encoderPin2 = 44;

// motors
const int X_motor_step = A4;
const int X_motor_dir = A3;
const int X_motor_ena = A5;

const int Y_motor_step = 5;
const int Y_motor_dir = 17;
const int Y_motor_ena = 4;

const int S_motor_step = 16;
const int S_motor_dir = 11;
const int S_motor_ena = 3;

const int F1_motor_step = 28;
const int F1_motor_dir = 27;
const int F1_motor_ena = 29;

const int F2_motor_step = 25;
const int F2_motor_dir = 24;
const int F2_motor_ena = 26;

const int B_motor_step = 22;
const int B_motor_dir = A6;
const int B_motor_ena = 23;


// endstops
const int X_endstop = 37;
const int Y_endstop = 40;
const int S_endstop = 41;
const int F1_endstop = 38;
const int F2_endstop = 18;
const int B_endstop = 19;

//buzzer
const int buzzer = A7;

//inkjet
const int aux1 = 46;
const int aux2 = 47;
const int aux3 = 48;
const int aux4 = 49;
const int nozzle_power_safety = 10;

//heating
const int B_sens = A13;
const int Bed_heater = 8;




//variables
//input variables
boolean switchUp, switchDown, switchLeft, switchRight, switchEnter, switchMenu1, switchMenu2, switchMenu3;

boolean Encoder1history;
boolean Encoder2history;
int Encoderdirection;

//lcd custom characters
byte pointer[8] = {
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000,
};

byte Bee1[8] = {
  B11111,
  B11111,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11111,
};

byte Bee3[8] = {
  B11111,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11111,
  B11111,
};

byte Bee2[8] = {
  B10000,
  B11000,
  B01100,
  B00110,
  B00110,
  B00110,
  B01100,
  B11000,
};

byte Bee4[8] = {
  B11100,
  B00110,
  B00011,
  B00011,
  B00011,
  B00110,
  B11100,
  B11000,
};

byte Degree[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};

//SD_read variables
boolean end_of_file = 0;
boolean end_of_print = 0;
boolean New_line = 0;
int export_buffer[5]; //export buffer
word nozzle_export = 0; //nozzle export
boolean Nozzle_export[14];

//coordinate trackers
int X_step_pos;
int Y_step_pos;
int F1_step_pos;
int F2_step_pos;
int B_step_pos;
int S_step_pos;
int E_10um_pos; //E is only tracked for nozzle purposes, so it doesn't need step resolution
int Z_history;

//coordinate targets
int X_step_target;
int Y_step_target;

//state trackers
boolean GHomed = 0;
boolean SHomed = 0;
boolean F1Homed = 0;
boolean F2Homed = 0;
boolean BHomed = 0;
boolean Out_of_feed_material = 0;

//Buffer variables
boolean Slowdown = 0;
int buffer_coordinate_read[5];
word buffer_nozzle_read;
boolean look_ahead_failure = 0;
int empty_slots = 0;

//movement variables
boolean target_reached;
boolean gantry_safe;
boolean X_constant_movement; //defines when X is on targeted movement (0) or constant movement (1)
boolean single_nozzle_printing;
int smart_NL_F1_pos = 0;
int smart_NL_F2_pos = 0;

//nozzle variables
boolean nozzle_target_reached;
boolean nozzle_safe;

//printing and estimate variables
boolean forced_stop = 0;
word number_of_layers;
unsigned long printing_time_estimate;




void setup() 
{  
  lcd.begin(20, 4);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);

  pinMode(X_motor_step, OUTPUT);
  pinMode(X_motor_dir, OUTPUT);
  digitalWrite(X_motor_ena, 0);
  pinMode(X_motor_ena, INPUT);

  pinMode(Y_motor_step, OUTPUT);
  pinMode(Y_motor_dir, OUTPUT);
  digitalWrite(Y_motor_ena, 0);
  pinMode(Y_motor_ena, INPUT);

  pinMode(S_motor_step, OUTPUT);
  pinMode(S_motor_dir, OUTPUT);
  digitalWrite(S_motor_ena, 0);
  pinMode(S_motor_ena, INPUT);

  pinMode(F1_motor_step, OUTPUT);
  pinMode(F1_motor_dir, OUTPUT);
  digitalWrite(F1_motor_ena, 0);
  pinMode(F1_motor_ena, INPUT);

  pinMode(F2_motor_step, OUTPUT);
  pinMode(F2_motor_dir, OUTPUT);
  digitalWrite(F2_motor_ena, 0);
  pinMode(F2_motor_ena, INPUT);

  pinMode(B_motor_step, OUTPUT);
  pinMode(B_motor_dir, OUTPUT);
  digitalWrite(B_motor_ena, 0);
  pinMode(B_motor_ena, INPUT);

  pinMode(X_endstop, INPUT);
  pinMode(Y_endstop, INPUT);
  pinMode(S_endstop, INPUT);
  pinMode(F1_endstop, INPUT);
  pinMode(F2_endstop, INPUT);
  pinMode(B_endstop, INPUT);

  pinMode(buzzer, OUTPUT);

  pinMode(B_sens, INPUT);
  pinMode(Bed_heater, OUTPUT);

  //inkjet
  pinMode(aux1, OUTPUT);
  pinMode(aux2, OUTPUT);
  pinMode(aux3, OUTPUT);
  pinMode(aux4, OUTPUT);

  //Inkjet uses direct port manupilation, so DDRL is used for defining that
  DDRL = DDRL | B1111;

  pinMode(nozzle_power_safety, OUTPUT);
  digitalWrite(nozzle_power_safety, LOW);



  //set encoder history
  Encoder1history = digitalRead(encoderPin1);
  Encoder2history = digitalRead(encoderPin2);

  //LCD characters
  lcd.createChar(1, pointer);
  lcd.createChar(2, Bee1);
  lcd.createChar(3, Bee2);
  lcd.createChar(4, Bee3);
  lcd.createChar(5, Bee4);
  lcd.createChar(6, Degree);

  clear_line_data();
  clear_export_buffer();
  clear_nozzle_history();
  clear_nozzle_stated();

  // initialize serial communication at 9600 bits per second
  Serial.begin(9600);

  Disable_All();
}




void loop()
{  
  update_menu();
}












//convert and read code
/*while (end_of_print == 0)
 {
 for (int z = 0; z < 20; z++)
 {
 buffer_update();
 }
 buffer_next();
 
 if (end_of_print == 0)
 {
 Serial.print(buffer_coordinate_read[0]);
 Serial.print(" ");
 Serial.print(buffer_coordinate_read[1]);
 Serial.print(" ");
 Serial.print(buffer_coordinate_read[2]);
 Serial.print(" ");
 Serial.print(buffer_coordinate_read[3]);
 Serial.print(" ");
 Serial.print(buffer_coordinate_read[4]);
 Serial.print(" ");
 for (int r = 2; r < 14; r++)
 {
 Serial.print(bitRead(buffer_nozzle_read,r));  
 }  
 Serial.println(" "); 
 }
 }
 
 
 
 
 
 
 
 
 //manual targeted movement
 inputs_update();
 gantry_update();
 
 if(switchEnter)
 {
 Home_gantry();
 }
 else if(switchUp)
 {
 gantry_set_target(5000,10000,50);
 }
 else if(switchDown)
 {
 gantry_set_target(5000,5000,50);
 }
 else if(switchLeft)
 {
 gantry_set_target(10000,5000,50);
 }
 else if(switchRight)
 {
 gantry_set_target(10000,10000,50);
 }
 
 if (target_timee < millis())
 {
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print(X_step_pos);
 lcd.setCursor(10,0);
 lcd.print(X_step_target);
 
 lcd.setCursor(0,1);
 lcd.print(Y_step_pos);
 lcd.setCursor(10,1);
 lcd.print(Y_step_target);
 target_timee += 1000;
 
 lcd.setCursor(10,3);
 lcd.print(target_reached);
 }*/


































