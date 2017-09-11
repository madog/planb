/*
This tab carries within it everything to render and navigate the menu's. There is a whole list of cases which define every posible pointer and tab in the menu. In every 
 tab these is also all posible button actions for that specific part of the menu.
 */

//amount of cycles menu stayes locked after all buttons are released
const byte Lock_time = 10;

int menuTab = 0;

byte previous_movement_menu = 1;
byte previous_homing_menu = 1;
byte previous_set_motion_menu = 1;
byte previous_printing_menu = 1;
byte previous_piston_setting = 1;
byte previous_heating_menu = 1;
byte previous_thickness = 10;

word layer_thickness = 10;
unsigned long delayed_update;
const int update_delay = 200; //in milliseconds
word printing_density = 100;
word heater_temperature = 20; 
word heater_history;
unsigned long heating_timer = millis();
unsigned int heating_framerate = 2000;


void menu_lock()
{
  byte lock_time = Lock_time;
  while (lock_time > 0)
  {
    inputs_update();
    if (switchUp == 0 && switchDown == 0 && switchLeft == 0 && switchRight == 0 && switchEnter == 0 && switchMenu1 == 0 && switchMenu2 == 0 && switchMenu3 == 0)
    {
      lock_time --;
    }
  }
}


void update_menu()
{
  //read shift registers
  inputs_update();

  //update heating
  if (millis() > heating_timer)
  {
    update_temperature();
    heating_timer += heating_framerate;
  }

  switch (menuTab) 
  {
    //menu opening scene
  case 0:
    lcd.setCursor(13,0);
    lcd.print("Plan-");
    lcd.write(2);
    lcd.write(3);
    lcd.setCursor(18,1);
    lcd.write(4);
    lcd.write(5);
    lcd.setCursor(0,0);
    lcd.print("Y-Tec");
    lcd.setCursor(8,3);
    lcd.print("Version 0.05");


    menuTab = 1;
    break;

  case 1:
    //if anything, continue
    if(switchUp || switchDown || switchLeft || switchRight || switchEnter || switchMenu1 || switchMenu2 || switchMenu3)
    {
      menuTab = 2;
    }
    break;


    //Movement selector
  case 2:
    menuTab = 3;

    lcd.clear();
    lcd.write(1);
    lcd.print(" Movement");
    lcd.setCursor(0,1);
    lcd.print("  Homing");
    lcd.setCursor(0,2);
    lcd.print("  Set Motion");
    lcd.setCursor(0,3);
    lcd.print("  Printing");
    menu_lock();
    break;


  case 3:
    if (switchDown || Encoderdirection > 0)
    {
      menuTab = 4;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 10;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      switch(previous_movement_menu)
      {
      case 1:
        menuTab = 22;
        break;
      case 2:
        menuTab = 24;
        break;
      case 3:
        menuTab = 26;
        break;
      case 4:
        menuTab = 28;
        break;
      }
    }
    break;


    //homing selector
  case 4:
    menuTab = 5;

    lcd.clear();
    lcd.print("  Movement");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Homing");
    lcd.setCursor(0,2);
    lcd.print("  Set Motion");
    lcd.setCursor(0,3);
    lcd.print("  Printing");
    menu_lock();
    break;

  case 5:
    if (switchDown || Encoderdirection > 0)
    {
      menuTab = 6;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 2;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      switch(previous_homing_menu)
      {
      case 1:
        menuTab = 42;
        break;
      case 2:
        menuTab = 44;
        break;
      case 3:
        menuTab = 46;
        break;
      case 4:
        menuTab = 48;
        break;
      case 5:
        menuTab = 50;
        break;
      case 6:
        menuTab = 52;
        break;          
      }
    }

    break;


    //Set motion selector
  case 6:
    menuTab = 7;

    lcd.clear();
    lcd.print("  Homing");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Set Motion");
    lcd.setCursor(0,2);
    lcd.print("  Printing");
    lcd.setCursor(0,3);
    lcd.print("  Heating");
    menu_lock();
    break;

  case 7:
    if (switchDown || Encoderdirection > 0)
    {
      menuTab = 8;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 4;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      switch(previous_set_motion_menu)
      {
      case 1:
        menuTab = 72;
        break;
      case 2:
        menuTab = 74;
        break;
      }

    }

    break;


    //Printing selector
  case 8:
    menuTab = 9;
    lcd.clear();
    lcd.print("  Homing");
    lcd.setCursor(0,1);
    lcd.print("  Set Motion");
    lcd.setCursor(0,2);
    lcd.write(1);
    lcd.print(" Printing");
    lcd.setCursor(0,3);
    lcd.print("  Heating");
    menu_lock();
    break;

  case 9:
    if (switchDown || Encoderdirection > 0)
    {
      menuTab = 10;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 6;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      switch(previous_printing_menu)
      {
      case 1:
        menuTab = 92;
        break;
      case 2:
        menuTab = 94;
        break;
      case 3:
        menuTab = 96;
        break;
      }
    }
    break;


  case 10:
    menuTab = 11;

    lcd.clear();
    lcd.print("  Homing");
    lcd.setCursor(0,1);
    lcd.print("  Set Motion");
    lcd.setCursor(0,2);
    lcd.print("  Printing");
    lcd.setCursor(0,3);
    lcd.write(1);
    lcd.print(" Heating");
    menu_lock();
    break;

  case 11:
    if (switchDown || Encoderdirection > 0)
    {
      menuTab = 2;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 8;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      switch(previous_heating_menu)
      {
      case 1:
        menuTab = 112;
        break;
      case 2:
        menuTab = 114;
        break;
      case 3:
        menuTab = 116;
        break;
      }
    } 
    break;


    //movement
    //gantry movement
  case 22:
    menuTab = 23;
    previous_movement_menu = 1;
    lcd.clear();
    lcd.write(1);
    lcd.print(" Gantry movement");
    lcd.setCursor(0,1);
    lcd.print("  Spreader movement");
    lcd.setCursor(0,2);
    lcd.print("  Feed pistons");
    lcd.setCursor(0,3);
    lcd.print("  Build piston");
    Disable_All();
    menu_lock();
    break;

  case 23:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 2;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 24;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 28;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 222;
    }
    break;

    //roller movement
  case 24:
    menuTab = 25;
    previous_movement_menu = 2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Gantry movement");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Spreader movement");
    lcd.setCursor(0,2);
    lcd.print("  Feed pistons");
    lcd.setCursor(0,3);
    lcd.print("  Build piston");
    Disable_All();
    menu_lock();
    break;

  case 25:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 2;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 26;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 22;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 242;
    }
    break;



    //Piston movement
    //Feed pistons
  case 26:
    menuTab = 27;
    previous_movement_menu = 3;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Gantry movement");
    lcd.setCursor(0,1);
    lcd.print("  Spreader movement");
    lcd.setCursor(0,2);
    lcd.write(1);
    lcd.print(" Feed pistons");
    lcd.setCursor(0,3);
    lcd.print("  Build piston");
    Disable_All();
    menu_lock();
    break;

  case 27:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 2;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 28;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 24;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 262;
    }
    break;
    //build piston
  case 28:
    menuTab = 29;
    previous_movement_menu = 4;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Gantry movement");
    lcd.setCursor(0,1);
    lcd.print("  Spreader movement");
    lcd.setCursor(0,2);
    lcd.print("  Feed pistons");
    lcd.setCursor(0,3);
    lcd.write(1);
    lcd.print(" Build piston");
    Disable_All();
    menu_lock();
    break;

  case 29:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 2;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 22;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 26;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 282;
    }
    break;



    //homing menu
    //home gantry
  case 42:
    menuTab = 43;
    previous_homing_menu = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.print(" Home Gantry");
    lcd.setCursor(0,1);
    lcd.print("  Home Spreader");
    lcd.setCursor(0,2);
    lcd.print("  Home Build piston");
    lcd.setCursor(0,3);
    lcd.print("  Home Feed piston 1");
    menu_lock();
    break;

  case 43:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 44;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 52;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 422;
    }
    break;


    //home roller
  case 44:
    menuTab = 45;
    previous_homing_menu = 2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Home Gantry");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Home Spreader");
    lcd.setCursor(0,2);
    lcd.print("  Home Build piston");
    lcd.setCursor(0,3);
    lcd.print("  Home Feed piston 1");
    menu_lock();
    break;

  case 45:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 46;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 42;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 442;
    }
    break;


    //Home B
    //Sure?
  case 46:
    menuTab = 47;
    previous_homing_menu = 3;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Home Spreader");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Home Build piston");
    lcd.setCursor(0,2);
    lcd.print("  Home Feed piston 1");
    lcd.setCursor(0,3);
    lcd.print("  Home Feed piston 2");
    menu_lock();
    break;

  case 47:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 48;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 44;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 462;
    }
    break;


    //Home F1
    //sure?
  case 48:
    menuTab = 49;
    previous_homing_menu = 4;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Home Build piston");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Home Feed piston 1");
    lcd.setCursor(0,2);
    lcd.print("  Home Feed piston 2");
    lcd.setCursor(0,3);
    lcd.print("  Home All");
    menu_lock();
    break;

  case 49:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 50;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 46;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 482;
    }
    break;


    //home F2
    //Sure?
  case 50:
    menuTab = 51;
    previous_homing_menu = 5;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Home Build piston");
    lcd.setCursor(0,1);
    lcd.print("  Home Feed piston 1");
    lcd.setCursor(0,2);
    lcd.write(1);
    lcd.print(" Home Feed piston 2");
    lcd.setCursor(0,3);
    lcd.print("  Home All");
    menu_lock();
    break;

  case 51:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 52;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 48;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 502;
    }
    break;


    //home all pistons
    //Sure?
  case 52:
    menuTab = 53;
    previous_homing_menu = 6;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Home Build piston");
    lcd.setCursor(0,1);
    lcd.print("  Home Feed piston 1");
    lcd.setCursor(0,2);
    lcd.print("  Home Feed piston 2");
    lcd.setCursor(0,3);
    lcd.write(1);
    lcd.print(" Home All");
    menu_lock();
    break;

  case 53:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 4;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 42;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 50;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchRight)
    {
      menuTab = 522;
    }
    break;



    //Set motions
    //Add layer
    // thickness
  case 72:
    menuTab = 73;
    previous_set_motion_menu = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.print(" Add layer");
    lcd.setCursor(0,1);
    lcd.print("  Sw. Spreader side");
    menu_lock();
    break;

  case 73:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 6;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 74;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 74;
      Encoderdirection = 0;
    }
    else if (switchEnter)
    {
      menuTab = 722;
    }
    break;


    //Roller side switch
  case 74:
    menuTab = 75;
    previous_set_motion_menu = 2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Add layer");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Sw. Spreader side");
    menu_lock();
    break;

  case 75:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 6;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 72;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 72;
      Encoderdirection = 0;
    }
    break;

    //printing menu
    //Estimate
    //time, layers, feed distance needed
    ///estimate and check code
  case 92:
    menuTab = 93;
    previous_printing_menu = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.print(" Estimate");
    lcd.setCursor(0,1);
    lcd.print("  Print");
    lcd.setCursor(0,2);
    lcd.print("  PCB Print");
    menu_lock();
    break;

  case 93:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 8;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 94;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 96;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchLeft)
    {
      menuTab = 922;
    }
    break;

    //print
    //sure?
    ///estimate and check code
  case 94:
    menuTab = 95;
    previous_printing_menu = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Estimate");
    lcd.setCursor(0,1);
    lcd.write(1);
    lcd.print(" Print");
    lcd.setCursor(0,2);
    lcd.print("  PCB Print");
    menu_lock();
    break;

  case 95:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 8;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 96;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 92;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchLeft)
    {
      menuTab = 942;
    }
    break;

    //print
    //sure?
    ///estimate and check code
  case 96:
    menuTab = 97;
    previous_printing_menu = 3;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Estimate");
    lcd.setCursor(0,1);
    lcd.print("  Print");
    lcd.setCursor(0,2);
    lcd.write(1);
    lcd.print(" PCB Print");
    menu_lock();
    break;

  case 97:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 8;
    }
    else if (switchDown || Encoderdirection > 0)
    {
      menuTab = 92;
      Encoderdirection = 0;
    }
    else if (switchUp || Encoderdirection < 0)
    {
      menuTab = 94;
      Encoderdirection = 0;
    }
    else if (switchEnter || switchLeft)
    {
      menuTab = 962;
    }
    break;



    //heating menu
    //preheat printer
    //Read temperature


  case 112:
    menuTab = 113;
    previous_heating_menu = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write(1);
    lcd.print(" Set temperature");
    lcd.setCursor(2,3);
    lcd.print(build_sensor_temperature);
    lcd.write(6);
    lcd.print("C / ");
    lcd.print(heater_temperature);
    lcd.write(6);
    lcd.print("C");
    delayed_update = millis();
    menu_lock();
    break;

  case 113:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 10;
    }
    else if (switchEnter || switchLeft)
    {
      menuTab = 1122;
    }
    if (millis() - delayed_update < 10000000)
    {
      lcd.setCursor(2,3);
      lcd.print(build_sensor_temperature);
      lcd.write(6);
      lcd.print("C");
      delayed_update += update_delay;
    }
    break; 








  case 222: //----------------------------------------------------------------------------------------------------------------------------------------------------------
    menuTab = 223;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Gantry Active");
    Enable_X();
    Enable_Y();
    menu_lock();
    break;

  case 223:
    if (switchLeft)
    {
      if (digitalRead(Y_endstop) == 0)
      {
        Set_Y_direction(0);
        Set_X_direction(1);
        Step_Y();
        Step_X();
        delayMicroseconds(500);
      }
    }
    else if (switchRight)
    {
      Set_Y_direction(1);
      Set_X_direction(0);
      Step_Y();
      Step_X();
      delayMicroseconds(500);
    }
    else if (switchUp)
    {
      if (digitalRead(X_endstop) == 0)
      {
        Set_X_direction(0);
        Step_X();
        delayMicroseconds(500);
      }
    }
    else if (switchDown)
    {
      Set_X_direction(1);
      Step_X();
      delayMicroseconds(500);
    }

    if (switchMenu1)
    {
      menuTab = 22;
    }
    break;

  case 242:
    menuTab = 243;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Spreader Active");
    Enable_S();
    menu_lock();
    break;

  case 243:
    if (switchLeft)
    {
      if (digitalRead(S_endstop) == 0)
      {
        Set_S_direction(0);
        Step_S();
        delayMicroseconds(500);
      }
    }
    else if (switchRight)
    {
      Set_S_direction(1);
      Step_S();
      delayMicroseconds(500);
    }
    if (switchMenu1)
    {
      menuTab = 24;
    }
    break;

  case 262:
    menuTab = 263;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Feed pistons active");
    Enable_F1();
    Enable_F2();
    menu_lock();
    break;

  case 263:
    if (switchLeft)
    {
      Set_F1_direction(1);
      Step_F1();
      delayMicroseconds(150);
    }
    else if (switchRight)
    {
      if (digitalRead(F1_endstop) == 0)
      {
        Set_F1_direction(0);
        Step_F1();
        delayMicroseconds(150);
      }
    }
    else if (switchUp)
    {
      if (digitalRead(F2_endstop) == 0)
      {
        Set_F2_direction(0);
        Step_F2();
        delayMicroseconds(150);
      }
    }
    else if (switchDown)
    {
      Set_F2_direction(1);
      Step_F2();
      delayMicroseconds(150);
    }

    if (switchMenu1)
    {
      menuTab = 26;
    }
    break;

  case 282:
    menuTab = 283;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Build piston Active");
    Enable_B();
    menu_lock();
    break;

  case 283:
    if (switchUp)
    {
      if (digitalRead(B_endstop) == 0)
      {
        Set_B_direction(0);
        Step_B();
        delayMicroseconds(150);
      }
    }
    else if (switchDown)
    {
      Set_B_direction(1);
      Step_B();
      delayMicroseconds(150);
    }

    if (switchMenu1)
    {
      menuTab = 28;
    }
    break;



    //Homing Submenu
  case 422:
    menuTab = 423;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing Gantry");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 423:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 42;
    }
    else if (switchUp)
    {
      menuTab = 424;
    }
    break;

  case 424:
    menuTab = 425;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 425:
    Home_gantry();
    Disable_X();
    Disable_Y();
    menuTab = 42;
    break;  

  case 442:
    menuTab = 443;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing Spreader");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 443:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 44;
    }
    else if (switchUp)
    {
      menuTab = 444;
    }
    break;

  case 444:
    menuTab = 445;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 445:
    Home_S();
    Disable_S();
    menuTab = 44;
    break;  

  case 462:
    menuTab = 463;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing Build");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 463:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 46;
    }
    else if (switchUp)
    {
      menuTab = 464;
    }
    break;

  case 464:
    menuTab = 465;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 465:
    Home_B();
    Disable_B();
    menuTab = 46;
    break;      

  case 482:
    menuTab = 483;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing Feed 1");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 483:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 48;
    }
    else if (switchUp)
    {
      menuTab = 484;
    }
    break;

  case 484:
    menuTab = 485;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 485:
    Home_F1();
    Disable_F1();
    menuTab = 48;
    break;  

  case 502:
    menuTab = 503;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing Feed 2");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 503:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 50;
    }
    else if (switchUp)
    {
      menuTab = 504;
    }
    break;

  case 504:
    menuTab = 505;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 505:
    Home_F2();
    Disable_F2();
    menuTab = 50;
    break;  

  case 522:
    menuTab = 523;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Homing ALL!");
    lcd.setCursor(0,2);
    lcd.print("   Are you sure?");
    menu_lock();
    break;

  case 523:
    if (switchMenu1 || switchLeft || switchDown)
    {
      menuTab = 52;
    }
    else if (switchUp)
    {
      menuTab = 524;
    }
    break;

  case 524:
    menuTab = 525;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("    Homing");
    lcd.setCursor(0,2);
    lcd.print("   Press < and >");
    lcd.setCursor(0,3);
    lcd.print("    to interupt");
    break;

  case 525:
    Home_gantry();
    Home_S();
    Home_B();
    Home_F1();
    Home_F2();  
    Disable_All();
    menuTab = 52;
    break;  



    //set movements submenu
  case 722:
    menuTab = 723;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Add new layer");
    lcd.setCursor(0,2);
    lcd.print("  Layer thickness?");
    delayed_update = millis() + update_delay;
    lcd.setCursor(0,3);
    lcd.print("                   ");
    lcd.setCursor(2,3);
    lcd.print(layer_thickness * 10);
    lcd.setCursor(6,3);
    lcd.print("um   > add >");
    menu_lock();
    break;

  case 723:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 72;
    }
    else if (switchRight)
    {
      New_layer(0,0,layer_thickness);
      menuTab = 72;
    }
    if (switchDown || Encoderdirection < 0)
    {
      if (layer_thickness >= 2)
      {
        layer_thickness -= 1;
        if (switchDown)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    else if (switchUp  || Encoderdirection > 0)
    {
      if (layer_thickness < 100)
      {
        layer_thickness += 1;
        if (switchUp)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    if (millis() - delayed_update < 10000000)
    {
      lcd.setCursor(0,3);
      lcd.print("        ");
      lcd.setCursor(2,3);
      lcd.print(layer_thickness * 10);
      lcd.setCursor(6,3);
      lcd.print("um");
      delayed_update += update_delay;
    }
    break;



    //printing menu submenu's
  case 922:
    menuTab = 923;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Estimate print");
    lcd.setCursor(0,2);
    lcd.print("< Back    Continue >");
    menu_lock();
    break;

  case 923:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 92;
    }
    else if (switchRight)
    {
      menuTab = 924;
    }        
    break;

  case 924:
    menuTab = 925;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Estimating time");
    lcd.setCursor(0,2);
    lcd.print("layer 1");

    start_estimate();

    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Layers: ");
    lcd.print(number_of_layers);
    lcd.setCursor(0,2);
    lcd.print("Printing time: ");


    { //brackets needed for jump to case label error
      printing_time_estimate /= 1000;
      int time_minutes = printing_time_estimate % 3600;
      time_minutes /= 60;
      int time_hours = printing_time_estimate / 3600;

      if (time_hours < 10)
      {
        lcd.print("0");
      }
      lcd.print(time_hours);
      lcd.print(":");
      if (time_minutes < 10)
      {
        lcd.print("0");
      }
      lcd.print(time_minutes);
      break;
    }
  case 925:

    if (switchUp || switchDown || switchLeft || switchRight || switchEnter || switchMenu1 || switchMenu2 || switchMenu3)
    {
      menuTab = 92;
    }
    break;   

  case 942:
    menuTab = 943;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("3D printing");
    lcd.setCursor(0,1);
    lcd.print("Inkjet density: 100%");
    lcd.setCursor(0,2);
    lcd.print("< Back       Print >");
    printing_density = 100;
    delayed_update = millis();
    menu_lock();
    break;

  case 943:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 94;
    }
    else if (switchRight)
    {
      menuTab = 944;
    }
    //code for print density
    if (switchDown || Encoderdirection < 0)
    {
      if (printing_density >= 59)
      {
        printing_density -= 10;
        if (switchDown)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    else if (switchUp  || Encoderdirection > 0)
    {
      if (printing_density < 990)
      {
        printing_density += 10;
        if (switchUp)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    if (millis() - delayed_update < 10000000)
    {
      lcd.setCursor(16,1);
      lcd.print("    ");
      lcd.setCursor(16,1);
      lcd.print(printing_density);
      lcd.print("%");
      delayed_update += update_delay;
    }
    break;

  case 944:
    start_print(1, printing_density);
    menuTab = 94;
    break;


  case 962:
    menuTab = 963;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PCB printing");
    lcd.setCursor(0,1);
    lcd.print("Inkjet density: 100%");
    lcd.setCursor(0,2);
    lcd.print("< Back       Print >");
    printing_density = 100;
    delayed_update = millis();
    menu_lock();
    break;

  case 963:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 96;
    }
    else if (switchRight)
    {
      menuTab = 964;
    }  

    //code for print density
    if (switchDown || Encoderdirection < 0)
    {
      if (printing_density >= 59)
      {
        printing_density -= 10;
        if (switchDown)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    else if (switchUp  || Encoderdirection > 0)
    {
      if (printing_density < 990)
      {
        printing_density += 10;
        if (switchUp)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    if (millis() - delayed_update < 10000000)
    {
      lcd.setCursor(16,1);
      lcd.print("    ");
      lcd.setCursor(16,1);
      lcd.print(printing_density);
      lcd.print("%");
      delayed_update += update_delay;     
    }
    break;

  case 964:
    start_print(2, printing_density);
    menuTab = 96;
    break;





  case 1122:
    menuTab = 1123;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Set temperature");
    lcd.setCursor(0,2);
    lcd.print("  ^ Up  v Down ");
    lcd.setCursor(2,3);
    lcd.print(heater_temperature);
    lcd.write(6);
    lcd.print("C    > Set >");
    delayed_update = millis();
    heater_history = heater_temperature;
    menu_lock();
    break;

  case 1123:
    if (switchMenu1 || switchLeft)
    {
      menuTab = 112;
      heater_temperature = heater_history;
    }
    else if (switchRight)
    {
      heater_history = heater_temperature;
      set_build_temperature(heater_temperature);
      menuTab = 112;
    }  
    if (switchDown || Encoderdirection < 0)
    {
      if (heater_temperature >= 11)
      {
        heater_temperature --;
        if (switchDown)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    else if (switchUp  || Encoderdirection > 0)
    {
      if (heater_temperature < 99)
      {
        heater_temperature ++;
        if (switchUp)
        {
          delay(100);
        }
      }
      Encoderdirection = 0;
    }
    if (millis() - delayed_update < 10000000)
    {
      lcd.setCursor(2,3);
      lcd.print(heater_temperature);
      lcd.write(6);
      lcd.print("C");
      delayed_update += update_delay;
    }
    break;


  }
}


