/*
The printing tab is responsible for everything needed to make a good print, it ties several tabs of commands together to make everything move and print at the right times.
 
 The estimate/check function is also stored as a separate entity so it can be addressed separately to preview a print and prepare the printer.
 
 There are two kinds of printing, normal printing and PCB printing.
 -Normal printing is 3D printing, it includes new layer commands and reads Z-coordinates.
 -PCB printing ignores new layers and Z-axis changes and only prints on a flat surface. PCB printing can be used to print on paper or mask PCB's for etching.
 */

//The distance X accelerates ahead of first nozzle target so speed can level out. 
const int X_acceleration_distance = 190; //in 10um (1000 = 10mm)

const byte printing_temperature = 50; //in degrees celcius
const int heat_cycle_time = 3600; //in seconds
const byte heat_cycle_temperature = 50; //in degrees celcius
int heat_cycle_countdown;

//starting time is used to store when the printer started, so the firmware can calculate how long it is printing
unsigned long print_starting_time; //overflows after 50 days, so beware :p
word layers_printed;
boolean SDfound = 0;





void start_print(byte print_mode, word inkjet_density)
{
  //calculate nozzle time targets based on standard density  
  long nozzle_calc_temp = X_print_speed;
  nozzle_calc_temp *= 1000000;
  nozzle_calc_temp /= 25400;
  nozzle_calc_temp *= printing_DPI;
  nozzle_calc_temp = 1000000000 / nozzle_calc_temp;

  //recalculate value, adding according to the given density
  nozzle_calc_temp *= 100;
  nozzle_calc_temp /= inkjet_density;

  //set nozzle delay time
  word nozzle_step_time = nozzle_calc_temp; 

  //History values, used to keep track of previous possitions.
  int X_pos_history = 0;
  int Y_pos_history = 0;
  int Z_pos_history = 0;
  boolean print_direction;

  int nozzle_step_set_target;
  unsigned long nozzle_deposit_target;

  //estimate print
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("  Estimating time");
  lcd.setCursor(0,2);
  lcd.print("layer 1 ");
  start_estimate();

  //open file "print.txt"
  if (SDfound == 0) //initialize the SD card, but only if it hasn't initiated before;
  {
    if (!SD.begin(53)) //<=============================================================================================================== allow to return to menu if this fails
    {
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("No SD-card found");
      lcd.setCursor(2,2);
      lcd.print("reset printer");
      return;
    }
  }
  SDfound = 1;
  end_of_file = 0;
  end_of_print = 0;  
  printFile = SD.open("print.txt");


  word current_layer = 1;

  //do 3 beepy noises to state printer is starting
  for (int b = 0; b < 3; b++)
  {
    digitalWrite(buzzer, 1);
    delay(150);
    digitalWrite(buzzer, 0);
    delay(350);
  }

  //store starting time, so printer can determine how long it took to print (in millis())
  print_starting_time = millis();




  //put all information on lcd screen
  lcd.clear();
  lcd.print("Printing");
  lcd.setCursor(0,2);
  lcd.print("Layer 1    of     ");
  lcd.setCursor(0,3);
  lcd.print("Time remaining   :  ");

  //add number of layers
  if (print_mode == 2)
  {
    lcd.setCursor(14,2);
    lcd.print("1");
  }
  else
  {
    lcd.setCursor(14,2);
    lcd.print(number_of_layers);
  }

  //add time remaining;
  unsigned long time_calc = millis();
  time_calc -= print_starting_time;
  time_calc = printing_time_estimate - time_calc;
  time_calc /= 1000;
  if (time_calc > 356400)
  {
    time_calc = 0;
  }
  int time_minutes = time_calc % 3600;
  time_minutes /= 60;
  int time_hours = time_calc / 3600;


  lcd.setCursor(15,3);
  lcd.print("  ");
  lcd.setCursor(15,3);
  if (time_hours < 10)
  {
    lcd.print("0");
  }
  lcd.print(time_hours);
  lcd.setCursor(18,3);
  lcd.print("  ");
  lcd.setCursor(18,3);
  if (time_minutes < 10)
  {
    lcd.print("0");
  }
  lcd.print(time_minutes);

  //reset forced stop (is used for a menu stop to skip post printing procedures
  forced_stop = 0;

  //home gantry and spreader
  Home_gantry();
  Home_S();

  //set printing temperature, but only if 3D printing
  if (print_mode == 1)
  {
    set_build_temperature(printing_temperature);
  }


  //fill buffer
  buffer_fill();



  //start print
  boolean printing = 1;

  digitalWrite(nozzle_power_safety, HIGH);

  while (printing)
  {  
    //check movement while targeted movement 
    if (target_reached == 1)
    {
      if (buffer_coordinate_read[0] == 1001 || buffer_coordinate_read[0] == 1002 || buffer_coordinate_read[0] == 1003) //if it is G1 (with or without direction)
      {
        byte temp_dir;
        X_stop();

        //set Y possition
        int temp_Y_pos;
        if (buffer_coordinate_read[2] != -1)
        {
          temp_Y_pos = buffer_coordinate_read[2];
          Y_pos_history = temp_Y_pos;
        }
        else 
        {
          temp_Y_pos = Y_pos_history;
        }

        //set Z possition
        int temp_Z_pos = buffer_coordinate_read[3];

        //set X possition
        int temp_X_pos;

        //read direction
        if (buffer_coordinate_read[0] == 1001) // without Direction, pure targeted movement, also write X
        {
          if (buffer_coordinate_read[1] != -1)
          {
            temp_X_pos = buffer_coordinate_read[1];
            X_pos_history = temp_X_pos;
          }
          else
          {
            temp_X_pos = X_pos_history;
          }  

          //check is E axis needs to move. Turn on Nozzles is it has to move forward, turn of otherwise
          if (buffer_coordinate_read[4] > E_10um_pos)
          {
            //turn single nozzle printing on
            single_nozzle_printing = 1;
            //set first deposition target
            nozzle_deposit_target = micros();

            //make E_10um_pos the buffer_read position
            E_10um_pos = buffer_coordinate_read[4];
          }
          else
          {
            //turn single nozzle printing off
            single_nozzle_printing = 0;
          }
          buffer_next();
        }
        else //with direction, additional information later in code needed. (no X i read, look ahead for X)
        {

          //write smart layer coordinate to lowest and highest position, depending on coordinate
          //if the smart layer positions haven't been written yet (=0), put coordinate in both
          if (smart_NL_F1_pos == 0 && smart_NL_F2_pos == 0)
          {
            smart_NL_F1_pos = temp_Y_pos;
            smart_NL_F2_pos = temp_Y_pos;
          }
          else
          {
            if (temp_Y_pos < smart_NL_F1_pos) //if Y target = smaller than smallest smart layer function, write new smart layer position
            {
              smart_NL_F1_pos = temp_Y_pos;
            }
            if (temp_Y_pos > smart_NL_F2_pos) //if Y target = bigger than biggest smart layer function, write new smart layer position
            {
              smart_NL_F2_pos = temp_Y_pos;
            }
          }

          if (buffer_coordinate_read[0] == 1002) //decode the direction from the G command, 1002 is G1, D0
          {
            temp_dir = 0;
          }
          else if (buffer_coordinate_read[0] == 1003) //1003 is G1, D1
          {
            temp_dir = 1;
          }

          byte look_ahead_temp = 5; //look ahead a maximum of 5 lines, If it is then still not found... (no consequences have been added yet)
          while (look_ahead_temp != 0)
          {
            buffer_look_ahead();

            if (buffer_coordinate_read[0] == 2000) //if nozzle command found
            {
              //write look ahead X value to X
              look_ahead_temp = 1;
              temp_X_pos = buffer_coordinate_read[1];

              //add or subtract acceleration distance
              if (temp_dir == 0)
              {
                temp_X_pos += X_acceleration_distance;
                print_direction = 0;
              } 
              else if (temp_dir == 1)
              {
                temp_X_pos -= X_acceleration_distance;
                print_direction = 1;
              }
            }

            look_ahead_temp--;
          }
          buffer_next();
        }

        //if Z is the same or smaller (smaller gets ignored) set gantry target with values
        if (temp_Z_pos == -1 || temp_Z_pos <= Z_pos_history)
        {
          if (single_nozzle_printing == 0)
          {
            gantry_set_target(temp_X_pos, temp_Y_pos, travel_speed);
          }
          else if (single_nozzle_printing == 1)
          {
            gantry_set_target(temp_X_pos, temp_Y_pos, single_nozzle_print_speed);
          }
        }
        //if Z changed, new layer with values         
        else if (temp_Z_pos > Z_pos_history)
        {
          if (print_mode == 1)
          {
            int temp_layer_thickness = temp_Z_pos;
            temp_layer_thickness -= Z_pos_history;
            Z_pos_history = temp_Z_pos;
            update_temperature();

            //update LCD
            //layers
            current_layer ++;
            lcd.setCursor(6,2);
            lcd.print("    ");
            lcd.setCursor(6,2);
            lcd.print(current_layer);

            //time
            time_calc = millis();
            time_calc -= print_starting_time;
            time_calc = printing_time_estimate - time_calc;
            time_calc /= 1000;
            if (time_calc > 356400)
            {
              time_calc = 0;
            }
            int time_minutes = time_calc % 3600;
            time_minutes /= 60;
            int time_hours = time_calc / 3600;


            lcd.setCursor(15,3);
            lcd.print("  ");
            lcd.setCursor(15,3);
            if (time_hours < 10)
            {
              lcd.print("0");
            }
            lcd.print(time_hours);
            lcd.setCursor(18,3);
            lcd.print("  ");
            lcd.setCursor(18,3);
            if (time_minutes < 10)
            {
              lcd.print("0");
            }
            lcd.print(time_minutes);

            New_layer(temp_X_pos, temp_Y_pos, temp_layer_thickness);
          }
          else if (print_mode == 2) //if PCB printing, stop after first layer
          {
            end_of_print = 1;

            digitalWrite(nozzle_power_safety, LOW);
            //clear file
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("   Emptying file");
            while(end_of_file == 0)
            {
              read_sd_card();
            } 
          }
        }
      }

      //if it is a nozzle command
      else if (buffer_coordinate_read[0] == 2000) 
      {
        //set X possition
        if (buffer_coordinate_read[1] != -1)
        {
          //calculate nozzle step target
          nozzle_step_set_target = map(buffer_coordinate_read[1],0,100,0,X_steps_per_mm);

          //set nozzle_deposit_target
          nozzle_deposit_target = micros();

          X_start(print_direction);
          //inputs_update();
        }
        else //no X coordinate means faulty line, next
        {
          buffer_next();
        }
      }

      else if (buffer_coordinate_read[0] == 1092) // G92, modify the read variables (mostly E coordinate)
      {
        //make 0 positions 0
        //commented because X, Y and Z should not be altered
        /*if (buffer_coordinate_read[1] != -1) //if X is mentioned
         {
         if (buffer_coordinate_read[1] == 0) //if X has to be 0
         {
         X_step_pos = 0;
         }
         else //if X has to be something that is not 0
         {
         //recalculate and write
         }
         }
         if (buffer_coordinate_read[2] != -1) //if Y is mentioned
         {
         if (buffer_coordinate_read[2] == 0) //if Y has to be 0
         {
         Y_step_pos = 0;
         }
         else //if Y has to be something that is not 0
         {
         //recalculate and write
         }
         }
         if (buffer_coordinate_read[3] != -1) //if Z is mentioned
         {
         if (buffer_coordinate_read[3] == 0) //if Z has to be 0
         {
         Z_step_pos = 0;
         }
         else //if Y has to be something that is not 0
         {
         //recalculate and write
         }
         }*/
        if (buffer_coordinate_read[4] != -1) //if E is mentioned
        {
          if (buffer_coordinate_read[4] == 0) //if E has to be 0
          {
            E_10um_pos = 0;
          }
          else //if E has to be something that is not 0
          {
            //recalculate and write (for now only 0 will do
          }
        }

        //refill buffer
        while (empty_slots > 0)
        {
          buffer_update();
        }
        buffer_next();
      }

      else //if buffer doesn't contain sensible information (like a comment line), just go to the next line.
      {
        buffer_next();
      }
    }

    //check nozzle states while constant movement
    if (X_constant_movement == 1)
    {
      if ((X_step_pos >= nozzle_step_set_target - printing_double_direction_offset && print_direction == 1) || (X_step_pos <= nozzle_step_set_target + printing_double_direction_offset && print_direction == 0)) //if pos has overtaken the nozzle target
      {        
        //set new nozzle state
        nozzle_set(buffer_nozzle_read);

        buffer_next();

        if (buffer_coordinate_read[0] == 2000)
        { 
          //recalculate nozzle_step_set_target
          int temp_X_pos;
          if (buffer_coordinate_read[1] != -1)
          {
            temp_X_pos = buffer_coordinate_read[1];

            //calculate nozzle step target
            int temp_step_set = map(temp_X_pos,0,100,0,X_steps_per_mm);

            //check for printing direction
            if (print_direction == 1)
            {
              //is direction correct, set target, else, next line
              if (nozzle_step_set_target < temp_step_set)
              {
                nozzle_step_set_target = temp_step_set;
              }
              else
              {
                buffer_next();
              }
            }
            else if (print_direction == 0)
            {
              if (nozzle_step_set_target > temp_step_set)
              {
                nozzle_step_set_target = temp_step_set;
              }
              else
              {
                buffer_next();
              }
            }

          }
        }
        else if (buffer_coordinate_read[0] == 1001 ||buffer_coordinate_read[0] == 1002 || buffer_coordinate_read[0] == 1003) //if it is a G1 while in constant movement, stop the constant movement
        {
          X_stop();
          inputs_update();
          update_temperature();
        }
      }



      //update nozzle ejection
      if (micros() - nozzle_deposit_target < 10000000)
      {
        nozzle_trigger();

        //set next target
        nozzle_deposit_target += nozzle_step_time;
      }
    }

    if (single_nozzle_printing == 1)
    {
      if (micros() - nozzle_deposit_target < 10000000)
      {
        nozzle_single_trigger();

        //set next target
        nozzle_deposit_target += nozzle_step_time;
      }
    }

    //Update the gantry motors
    gantry_update();

    //enough time left for buffer update
    if (gantry_safe == 1)
    {
      buffer_update();
    }

    //pause?
    if (switchLeft && switchRight)
    {
      pause_print();      
    }


    //if end of print reached, end print
    if (end_of_print)
    {      
      printing = 0;

      digitalWrite(nozzle_power_safety, LOW);

      //close file
      printFile.close();
      X_stop();
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("   return to home"); 

      //return gantry to home possition
      Home_gantry();
      Disable_All();

      //set lcd to print done
      lcd.clear();

      if (forced_stop == 0 && print_mode == 1) //do not calculate the time took when a forced stop occured
      {
        //display time it took to make the print
        //add time remaining;
        time_calc = millis();
        time_calc -= print_starting_time;
        time_calc /= 1000;

        //if it is more than 100 hours (highly unlikely)
        if (time_calc > 356400)
        {
          time_calc = 0;
        }
        int time_minutes = time_calc % 3600;
        time_minutes /= 60;
        int time_hours = time_calc / 3600;


        lcd.setCursor(0,0);
        lcd.print("Print took: ");
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

        for (int b = 0; b < 2; b++)
        {
          digitalWrite(buzzer, 1);
          delay(500);
          digitalWrite(buzzer, 0);
          delay(500);
        }
        lcd.setCursor(0,1);
        lcd.print("Running Heat Cycle");
        lcd.setCursor(0,3);
        lcd.print("< and > to interrupt");

        //set heat cycle countdown timer to start heating cycle
        heat_cycle_countdown = heat_cycle_time;
        set_build_temperature(heat_cycle_temperature);
      }
    }
  }

  if (forced_stop == 0)
  {
    //lock final menu
    boolean locked = 1;
    //unlock if PCB print (does not require additional heating)
    if (print_mode == 2)
    {
      locked = 0;
      menuTab = 96;
    }
    else
    {
      int time_seconds = heat_cycle_countdown % 3600;
      time_seconds /= 3600;
      time_minutes = heat_cycle_countdown % 3600;
      time_minutes /= 60;
      time_hours = heat_cycle_countdown / 3600;

      lcd.setCursor(0,2);
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
      lcd.print(":");
      if (time_seconds < 10)
      {
        lcd.print("0");
      }
      lcd.print(time_seconds);

      lcd.setCursor(9,2);
      lcd.print(build_sensor_temperature);
      lcd.write(6);
      lcd.print("C / ");
      lcd.print(heat_cycle_temperature);
      lcd.write(6);
      lcd.print("C");


      while (locked == 1)
      {
        inputs_update();
        //jump back to menu on left and right
        if ((switchLeft && switchRight) || heat_cycle_countdown <= 0)
        {
          menuTab = 94;
          locked = 0;
        }
        update_temperature();

        time_seconds = heat_cycle_countdown % 3600;
        time_seconds = time_seconds % 60;
        time_minutes = heat_cycle_countdown % 3600;
        time_minutes /= 60;
        time_hours = heat_cycle_countdown / 3600;

        lcd.setCursor(0,2);
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
        lcd.print(":");
        if (time_seconds < 10)
        {
          lcd.print("0");
        }
        lcd.print(time_seconds);

        lcd.setCursor(9,2);
        lcd.print(build_sensor_temperature);
        lcd.write(6);
        lcd.print("C / ");
        lcd.print(heat_cycle_temperature);
        lcd.write(6);
        lcd.print("C");


        heat_cycle_countdown --;
        delay(998); //offset by several milliseconds to compensate for read temperature and lcd print
      }
    }
    digitalWrite(buzzer, 1);
    delay(1000);
    digitalWrite(buzzer, 0);
    delay(1000);
    set_build_temperature(10);
  }
  else 
  {
    //deplete file 
  }
}






void start_estimate() //estimate function. Go through all lines and estimate the printing time that is needed.
{
  //variables for calculating distances
  int X_from, Y_from, Z_from;
  int X_to, Y_to, Z_to;
  number_of_layers = 1;
  printing_time_estimate = 0;



  if (SDfound == 0) //initialize the SD card, but only if it hasn't initiated before;
  {
    if (!SD.begin(53)) //<=============================================================================================================== allow to return to menu if this fails
    {
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("No SD-card found");
      lcd.setCursor(2,2);
      lcd.print("reset printer");
      return;
    }
  }
  SDfound = 1;
  end_of_file = 0;
  end_of_print = 0;  
  printFile = SD.open("print.txt");

  buffer_fill();

  while (end_of_print == 0)
  {
    //read a character
    for (int e = 0; e < 30; e++)
    {
      buffer_update();
    }


    //G1, no D
    if (buffer_coordinate_read[0] == 1001)
    {
      //X to to
      if (buffer_coordinate_read[1] != -1)
      {
        X_to = buffer_coordinate_read[1];
      }
      else
      {
        X_to = X_from;
      }
      //Y to to
      if (buffer_coordinate_read[2] != -1)
      {
        Y_to = buffer_coordinate_read[2];
      }
      else
      {
        Y_to = Y_from;
      }
      //Z to to
      if (buffer_coordinate_read[3] != -1)
      {
        Z_to = buffer_coordinate_read[3];
      }
      else
      {
        Z_to = Z_from;
      }
      for (int e = 0; e < 30; e++)
      {
        buffer_update();
      }
      buffer_next();
    }

    //G1 with D
    else if (buffer_coordinate_read[0] == 1002 || buffer_coordinate_read[0] == 1003)
    {
      //Y to to
      if (buffer_coordinate_read[2] != -1)
      {
        Y_to = buffer_coordinate_read[2];
      }
      else
      {
        Y_to = Y_from;
      }
      //Z to to
      if (buffer_coordinate_read[3] != -1)
      {
        Z_to = buffer_coordinate_read[3];
      }
      else
      {
        Z_to = Z_from;
      }

      //read new line
      for (int e = 0; e < 30; e++)
      {
        buffer_update();
      }
      buffer_next();

      //X to to
      if (buffer_coordinate_read[1] != -1)
      {
        X_to = buffer_coordinate_read[1];
      }
      else
      {
        X_to = X_from;
      }
    }

    //N code
    else if (buffer_coordinate_read[0] == 2000)
    {
      //while no G1,
      while (buffer_coordinate_read[0] != 1001 && buffer_coordinate_read[0] != 1002 && buffer_coordinate_read[0] != 1003)
      {
        for (int e = 0; e < 30; e++)
        {
          buffer_update();
        }
        buffer_next();
        if (buffer_coordinate_read[0] == 2000)
        {
          if (buffer_coordinate_read[1] != -1)
          {
            X_to = buffer_coordinate_read[1];
          }
        }
      }
    }

    else
    {
      for (int e = 0; e < 30; e++)
      {
        buffer_update();
      }
      buffer_next();
    }


    //new layer? new layer ++, time + new layer time;
    if (Z_to > Z_from)
    {
      number_of_layers++;
      printing_time_estimate += estimate_new_layer_time;
      lcd.setCursor(6,2);
      lcd.print(number_of_layers);
    }


    //calculate the length of the straight line the carriage will travel
    unsigned long etempX = X_to;
    etempX -= X_from;
    etempX = abs(etempX);
    unsigned long etempY = Y_to;
    etempY -= Y_from;
    etempY = abs(etempY);
    unsigned long etempXY = sqrt((etempX*etempX)+(etempY*etempY));

    //calculate travel time
    etempXY *= 10;
    etempXY /= estimate_speed;

    //++ time
    printing_time_estimate += etempXY;      

    //to > from
    X_from = X_to;
    Y_from = Y_to;
    Z_from = Z_to;

    //buffer_next();

  }
  printFile.close();
}







































