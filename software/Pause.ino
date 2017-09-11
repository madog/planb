/*
The pause tab takes care of the in print pause function. It keeps track of the right values and makes sure that the print can properly continue after the pause has ended. 
 It is full interruptive, meaning that other cycles will not run as long as the pause is running.
 */
boolean Paused;
const int pause_return_feedrate = 30;

void pause_print()
{
  Paused = 1;
  byte paused_menutab = 0;

  //note the time so the printing target time can be updated
  unsigned long pause_start_time = millis();

  //note the target and the position so that the printer can return to it's original location again and go to the previous target
  int pause_X_step_target = X_step_target;
  int pause_Y_step_target = Y_step_target;
  int pause_X_step_pos = X_step_pos;
  int pause_Y_step_pos = Y_step_pos;

  //pause cycle
  while (Paused == 1)
  {
    /*
     menu tabs:
     Resume printing
     set temperature
     move gantry
     home gantry
     move feed pistons 
     move build piston
     stop printing
     */
    inputs_update();

    switch (paused_menutab)
    {
    case 0: //resume printing, hold enter for 3 seconds to continue
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.write(1);
      lcd.print(" Resume Printing");
      lcd.setCursor(0,2);
      lcd.print("  Set Temperature");
      lcd.setCursor(0,3);
      lcd.print("  Move Gantry"); 
      paused_menutab = 1;
      menu_lock();
      break;

    case 1:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 2;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 12;
        Encoderdirection = 0;
      }
      else if (switchEnter) //resume print
      {
        byte temp_hold_counter = 31; 
        while (switchEnter == 1 && temp_hold_counter != 0)
        {
          inputs_update();
          temp_hold_counter --;
          if (temp_hold_counter == 1)
          {
            //recalculate end time to adjust for the time in the menu
            

            //go to old location if gantry location was moved 
            //stay in cycle as long as the gantry is not in it's old position
            if (pause_X_step_pos != X_step_pos || pause_Y_step_pos != Y_step_pos)
            {
              //recalculate and set target
              long temp_pause;
              temp_pause = pause_X_step_pos;
              temp_pause *= 100;
              temp_pause /= X_steps_per_mm;
              int temp_pause_X_pos = temp_pause;

              temp_pause = pause_Y_step_pos;
              temp_pause *= 100;
              temp_pause /= Y_steps_per_mm;
              int temp_pause_Y_pos = temp_pause;

              gantry_set_target(temp_pause_X_pos, temp_pause_Y_pos, pause_return_feedrate);
              while (target_reached == 0)
              {      
                gantry_update();
              }
            }

            //set old target
            long temp_pause;
            temp_pause = pause_X_step_target;
            temp_pause *= 100;
            temp_pause /= X_steps_per_mm;
            int temp_pause_X_pos = temp_pause;

            temp_pause = pause_Y_step_target;
            temp_pause *= 100;
            temp_pause /= Y_steps_per_mm;
            int temp_pause_Y_pos = temp_pause;

            gantry_set_target(temp_pause_X_pos, temp_pause_Y_pos, travel_speed);

            //release pause
            Paused = 0;
            
            //rewrite information on LCD screen
            
          }
          delay(100);
        }      
      }
      break;

    case 2: //Set temperature, modify the printing temperature
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Resume Printing");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.print(" Set Temperature");
      lcd.setCursor(0,3);
      lcd.print("  Move Gantry"); 
      paused_menutab = 3;
      menu_lock();
      break;

    case 3:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 4;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 0;
        Encoderdirection = 0;
      }
      else if (switchEnter) 
      {

      }
      break;

    case 4: //Manually move the gantry 
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Set Temperature");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.print(" Move Gantry");
      lcd.setCursor(0,3);
      lcd.print("  Home Gantry"); 
      paused_menutab = 5;
      menu_lock();
      break;

    case 5:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 6;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 2;
        Encoderdirection = 0;
      }
      else if (switchEnter) 
      {

      }
      break;

    case 6: //Home the gantry in case something went wrong (collisions for instance)(warning message and hold)
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Move Gantry");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.print(" Home Gantry");
      lcd.setCursor(0,3);
      lcd.print("  Move Feed Pistons"); 
      paused_menutab = 7;
      menu_lock();
      break;

    case 7:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 8;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 4;
        Encoderdirection = 0;
      }
      else if (switchEnter) 
      {

      }
      break;

    case 8: //Move feed pistons (very slowly and with warning message up front)
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Home Gantry");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.print(" Move Feed Pistons");
      lcd.setCursor(0,3);
      lcd.print("  Move Build Piston"); 
      paused_menutab = 9;
      menu_lock();
      break;

    case 9:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 10;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 6;
        Encoderdirection = 0;
      }
      else if (switchEnter) 
      {

      }
      break;

    case 10: //Move build piston (very slowly and with warning message up front)
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Move Feed Pistons");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.print(" Move Build Piston");
      lcd.setCursor(0,3);
      lcd.print("  Stop Printing"); 
      paused_menutab = 11;
      menu_lock();
      break;

    case 11:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 12;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 8;
        Encoderdirection = 0;
      }
      else if (switchEnter)
      {

      }
      break;

    case 12: //stop printing, break from printin cycle, hold for 3 seconds
      lcd.clear();
      lcd.print("    Print Paused   ");
      lcd.setCursor(0,1);
      lcd.print("  Move Feed Pistons");
      lcd.setCursor(0,2);
      lcd.print("  Move Build Piston");
      lcd.setCursor(0,3);
      lcd.write(1);
      lcd.print(" Stop Printing"); 
      paused_menutab = 13;
      menu_lock();
      break;

    case 13:
      if (switchDown || Encoderdirection > 0)
      {
        paused_menutab = 0;
        Encoderdirection = 0;
      }
      else if (switchUp || Encoderdirection < 0)
      {
        paused_menutab = 10;
        Encoderdirection = 0;
      }
      else if (switchEnter) //stop print
      {
        byte temp_hold_counter = 31; 
        while (switchEnter == 1 && temp_hold_counter != 0)
        {
          inputs_update();
            temp_hold_counter --;
          if (temp_hold_counter == 1)
          {
            //deplete the print buffer
            digitalWrite(nozzle_power_safety, LOW);
            //clear file
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("   Emptying file");
            while(end_of_file == 0)
            {
              read_sd_card();
            } 

            //notify the printing tab that this is a forced stop and a lot of functions can be skipped
            forced_stop = 1;

            //release pause
            Paused = 0; 
          }
        }
      }
      break;

    } 
  }
}














