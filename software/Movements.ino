/* 
 This tab contains all features to calculate and requist movements (but not control the motors, that happens in motor commands), It processes movement commands 
 and calculates delay times, start and stop points and acceleration. 
 
 There are special blocks for manual movement, set movements, targeted movements and printing movements. All vary greatly and have their own necessities.
 
 The new layer command is completely interruptive. It is a largely separate entity, relying only a little on other functions. This is because it needs to be modified
 for speed later on and has lots op complex movements. During the new layer command the code buffer will be replenished. This function is also added to the New layer command.
 */

//Axes steps per mm values
const int X_steps_per_mm = 20;
const int Y_steps_per_mm = 20;
const int S_steps_per_mm = 20;
const int Piston_steps_per_mm = 640;

//maximum speed for the gantry
const int G_max_speed = 150; //in mm per second



//safetime for gantry movement. Tracks when update buffer is allowed
const int gantry_safetime = 50; //in us


//New layer values
boolean Spreader_position;
const int NL_spreader_feed_speed = 75; //in millimeter per second
const int NL_spreader_build_speed = 30; //in millimeters per second
const int NL_gantry_speed = 60; //in millimeter per second
const int NL_piston_speed = 1; //in millimeter per second
const int NL_piston_overfeed = 1; //amount of 10 microns the feed pistons need to move in relation to the build piston.
const int NL_opposite_piston_down_distance = 50; //the amount the opposite feed piston moves down before spreading, in 10 microns.

//The coordinates of the locations where the spreader changes states
const int S_travel_distance = 260; //in millimeters
const int S_slowdown_F1_position = 0; //in 10um
const int S_slowdown_F2_position = 15000; //in 10um
const word S_slowdown_margin = 1500; //extra distance for slowdown, in 10um



//targeted movement variables
int X_target_steps;
int Y_target_steps;
int X_difference_steps;
int Y_difference_steps;
word X_difference_step_pos;
word Y_difference_step_pos;
unsigned long X_speed;
unsigned long Y_speed;
unsigned long X_time_target;
unsigned long Y_time_target;





void gantry_update()
{
  //update X axis
  if (X_difference_step_pos < X_difference_steps || X_constant_movement == 1)
  {
    if (micros() - X_time_target < 10000000)
    {
      Step_X();
      X_time_target += X_speed;
      if (X_constant_movement == 0)
      {
        X_difference_step_pos++;
      }
    }
  }

  //update Y axis
  if (Y_difference_step_pos < Y_difference_steps)
  {
    if (micros() - Y_time_target < 10000000)
    {
      Step_Y();
      Y_time_target += Y_speed;
      Y_difference_step_pos++;
    }
  }

  //check safetime
  if (X_time_target - micros() > gantry_safetime && Y_time_target - micros() > gantry_safetime)
  {
    gantry_safe = 1;
  }
  else
  {
    gantry_safe = 0;
  }

  //check target reached
  if (X_difference_step_pos >= X_difference_steps && Y_difference_step_pos >= Y_difference_steps && X_constant_movement == 0)
  {
    target_reached = 1;
  }
  else
  {
    target_reached = 0;
  }
}





void gantry_set_target(int X_10um, int Y_10um, int feedrate)
{
  if (target_reached == 1)
  {
    target_reached = 0;
    if (feedrate > G_max_speed)
    {
      feedrate = G_max_speed;
    }
    X_target_steps = map(X_10um, 0, 100, 0, X_steps_per_mm);
    Y_target_steps = map(Y_10um, 0, 100, 0, Y_steps_per_mm);

    if (X_target_steps != X_step_pos || Y_target_steps != Y_step_pos)
    {
      //if x or y changed, calculate how much steps have to be made
      X_difference_steps = X_target_steps - X_step_pos;
      int X_temp_steps = X_difference_steps;
      Y_difference_steps = Y_target_steps - Y_step_pos;
      X_difference_steps -= Y_difference_steps; //offset for linked X/Y axis

        //set direction based on change, positive or negative
      if (X_difference_steps > 0)
      {
        Set_X_direction(1);
      }
      else 
      {
        Set_X_direction(0);
      }

      if (Y_difference_steps > 0)
      {
        Set_Y_direction(1);
      }
      else 
      {
        Set_Y_direction(0);
      }

      //make difference values absolute, so length calculations can be done
      X_difference_steps = abs(X_difference_steps);
      Y_difference_steps = abs(Y_difference_steps);
      X_temp_steps = abs(X_temp_steps);

      //calculate the length of the straight line the carriage will travel
      unsigned long tempX = X_temp_steps;
      unsigned long tempY = Y_difference_steps;
      word tempXY = sqrt((tempX*tempX)+(tempY*tempY));

      //calculate the speed if the carriage would travel in a straigt line
      unsigned long delaytimeXY = 1000000 / feedrate; //is in mm/s

      //calculate the slowdown factor and apply it to the current X and Y delay times
      X_speed = delaytimeXY;
      X_speed *= tempXY;
      X_speed /= X_difference_steps;
      X_speed /= X_steps_per_mm;

      Y_speed = delaytimeXY;
      Y_speed *= tempXY;
      Y_speed /= Y_difference_steps;
      Y_speed /= Y_steps_per_mm;

      //Set next step time targets
      X_time_target = micros() + X_speed;
      Y_time_target = micros() + Y_speed;

      //reset step tracking variables
      X_difference_step_pos = 0;
      Y_difference_step_pos = 0;            
    }
  }  
}






void X_start(boolean X_start_dir)
{
  if (X_constant_movement == 0)
  {
    //calculate X_speed value (is print speed)
    Set_X_direction(X_start_dir);
    long temp = X_print_speed;
    temp *= X_steps_per_mm;
    X_speed = 1000000 / temp;
    
    //interuptive acceleration block
    //start acceleration in while loop and only stop the while when acceleration delay time exceeds X_speed
    unsigned long temp_acc_start_time = micros(); //set start time
    temp_acc_start_time -= acc_max_delay_time; //remove maximum delay to make the calculations function
    int temp_acc_step_delay = acc_max_delay_time;
    long acc_calc_temp; //temporary value for calculations
    while (temp_acc_step_delay > X_speed) //as long as the acceleration delay is lower than the target delay, keep accelerating
    {
      //step 
      Step_X();
      
      //recalculate delay time
      acc_calc_temp = micros() - temp_acc_start_time; //us
      acc_calc_temp = constant_movement_X_acceleration * acc_calc_temp; //mm/us
      acc_calc_temp /= 1000000; //mm/s
      acc_calc_temp *= X_steps_per_mm;
      acc_calc_temp = 1000000 / acc_calc_temp;
      temp_acc_step_delay = acc_calc_temp;
      
      //delay the calculated time
      delayMicroseconds(temp_acc_step_delay);
    }   

    //test first time target
    X_time_target = micros() + X_speed;

    X_constant_movement = 1;
  }
}


void X_stop()
{
  if (X_constant_movement == 1)
  {
    X_constant_movement = 0;
  
    //interuptive decceleration block  
    //decelerate before stopping. Start a while look and decelerate until the maximum delay time has been reached.
    unsigned long temp_acc_start_time = micros(); //set start time
    temp_acc_start_time -= X_speed; //remove X_speed to make the calculations function
    int temp_acc_step_delay = X_speed;
    long acc_calc_temp; //temporary value for calculations
    while (temp_acc_step_delay < acc_max_delay_time) //as long as the decceleration delay is lower than the max delay, keep deccelerating
    {
      //step 
      Step_X();
      
      //recalculate delay time
      acc_calc_temp = micros() - temp_acc_start_time; //us
      acc_calc_temp = constant_movement_X_acceleration * acc_calc_temp; //mm/us
      acc_calc_temp /= 1000000; //mm/s
      if (acc_calc_temp >= X_print_speed) //protection against negative values
      {
        acc_calc_temp = X_print_speed - 1;
      }
      acc_calc_temp = X_print_speed - acc_calc_temp;
      acc_calc_temp *= X_steps_per_mm;
      acc_calc_temp = 1000000 / acc_calc_temp;
      temp_acc_step_delay = acc_calc_temp;
      
      //delay the calculated time
      delayMicroseconds(temp_acc_step_delay);
    }       
  }
}






void New_layer(int NL_X_target, int NL_Y_target, int layer_thickness) //in 10um -------------------------------------------------------------------------------------
{
  int New_layer_ident = 0;
  if (SHomed == 1 && GHomed == 1) // are important axes homed?
  {
    Enable_All();
    boolean Pistons_moved = 0;
    Set_X_direction(0);

    //calculate piston steps
    unsigned long NL_temp;
    NL_temp = layer_thickness;
    NL_temp *= Piston_steps_per_mm;
    NL_temp /= 100;
    int B_steps = NL_temp;
    Set_B_direction(1);

    int F1_steps;
    int F2_steps;

    if (Spreader_position == 1)  // begint met + waardes, moet met layer thickness beginnen in een geval
    {
      NL_temp = layer_thickness;
      NL_temp *= 2;
      NL_temp += NL_piston_overfeed;
      NL_temp += NL_opposite_piston_down_distance;
      NL_temp *= Piston_steps_per_mm;
      NL_temp /= 100;
      F1_steps = NL_temp;
      Set_F1_direction(0);

      NL_temp = NL_opposite_piston_down_distance;
      NL_temp *= Piston_steps_per_mm;
      NL_temp /= 100;
      F2_steps = NL_temp;
      Set_F2_direction(1);
    }
    else
    {
      NL_temp = layer_thickness;
      NL_temp *= 2;
      NL_temp += NL_piston_overfeed;
      NL_temp += NL_opposite_piston_down_distance;
      NL_temp *= Piston_steps_per_mm;
      NL_temp /= 100;
      F2_steps = NL_temp;
      Set_F2_direction(0);

      NL_temp = NL_opposite_piston_down_distance;
      NL_temp *= Piston_steps_per_mm;
      NL_temp /= 100;
      F1_steps = NL_temp; 
      Set_F1_direction(1);
    }


    //calculate step delay times
    NL_temp = X_steps_per_mm;
    NL_temp *= NL_gantry_speed;
    int G_delay = 1000000 / NL_temp;

    NL_temp = S_steps_per_mm; 
    NL_temp *= NL_spreader_feed_speed;
    int S_feed_delay = 1000000 / NL_temp;

    NL_temp = S_steps_per_mm; 
    NL_temp *= NL_spreader_build_speed;
    int S_build_delay = 1000000 / NL_temp;

    NL_temp = Piston_steps_per_mm;
    NL_temp *= NL_piston_speed;
    int P_delay = 1000000 / NL_temp;


    unsigned long P_target = micros() + P_delay;
    unsigned long G_target = micros() + G_delay;

    int NL_B_target = 0;
    int NL_F1_target = 0;
    int NL_F2_target = 0;


    while (New_layer_ident == 0) //move pistons and move X out of the way
    {
      if (X_step_pos != X_offset) // is X axis already on endstop position (out of the way of spreader)
      {
        if (micros() - G_target < 10000000)
        {
          Step_X();
          G_target += G_delay;
        }
      }

      if (micros() - P_target < 10000000)
      {
        if (Pistons_moved == 0)
        {
          Pistons_moved = 1;
          if (NL_B_target < B_steps)
          {
            Step_B();
            NL_B_target ++;
            Pistons_moved = 0;
          }
          if (NL_F1_target < F1_steps)
          {
            Step_F1();
            NL_F1_target ++;
            Pistons_moved = 0;
          }
          if (NL_F2_target < F2_steps)
          {
            Step_F2();
            NL_F2_target ++;
            Pistons_moved = 0;
          }
          P_target += P_delay; 
        }
      }

      if (digitalRead(F1_endstop) == 1 || digitalRead(F2_endstop) == 1)
      {
        Out_of_feed_material = 1;
        //New_layer_ident = 30;
      }

      if (X_step_pos == X_offset && Pistons_moved == 1)
      {
        New_layer_ident = 10;
      }
    }



    //Y-step target
    long NL_postemp = NL_Y_target;
    NL_postemp *= Y_steps_per_mm;
    NL_postemp /= 100;
    int NL_Y_target_steps = NL_postemp;
    if (NL_Y_target_steps == Y_step_pos)
    {
    }
    else if (NL_Y_target_steps > Y_step_pos)
    {
      Set_Y_direction(1);
      Set_X_direction(0);
    }
    else 
    {
      Set_Y_direction(0);
      Set_X_direction(1);
    }



    //S-step target
    int NL_S_final_target_steps;
    int NL_S_slowdown_target_steps;
    int NL_S_speedup_target_steps;

    if (Spreader_position == 0)
    {
      NL_postemp = S_travel_distance;
      NL_postemp *= S_steps_per_mm;
      NL_S_final_target_steps = NL_postemp;

      Set_S_direction(1);

    }
    else 
    {
      NL_postemp = S_offset;
      NL_S_final_target_steps = NL_postemp;
      Set_S_direction(0);
    }

    //set smart layer values to stardard values if coordinates haven't been updated (still 0)
    if (smart_NL_F1_pos == 0 && smart_NL_F2_pos == 0)
    {
      smart_NL_F1_pos = S_slowdown_F1_position;
      smart_NL_F2_pos = S_slowdown_F2_position;
    }
    //add margin so the product isn't touched
    smart_NL_F1_pos -= S_slowdown_margin;
    smart_NL_F2_pos += S_slowdown_margin;

    //set the slowdown and speedup targets, where the spreader goed from fast to slow and back again
    NL_postemp = smart_NL_F1_pos;
    NL_postemp *= S_steps_per_mm;
    NL_postemp /= 100;
    NL_S_slowdown_target_steps = NL_postemp;

    NL_postemp = smart_NL_F2_pos;
    NL_postemp *= S_steps_per_mm;
    NL_postemp /= 100;
    NL_S_speedup_target_steps = NL_postemp;



    G_target = micros() + G_delay;
    unsigned long S_target = micros() + S_feed_delay;

    while (New_layer_ident == 10) // move Y and S to set targets
    {

      if (Y_step_pos != NL_Y_target_steps)
      {
        if (micros() - G_target < 10000000)
        {
          Step_Y();
          Step_X();
          G_target += G_delay;
        }
      }

      if (S_step_pos != NL_S_final_target_steps)
      {
        if (micros() - S_target < 10000000)
        {
          Step_S();
          if (S_step_pos > NL_S_slowdown_target_steps && S_step_pos < NL_S_speedup_target_steps)
          {
            S_target += S_build_delay;
          }
          else
          {
            S_target += S_feed_delay;
          }
        }
      }

      if (Y_step_pos == NL_Y_target_steps && S_step_pos == NL_S_final_target_steps)
      {
        New_layer_ident = 20;
        if (Spreader_position == 0)
        {
          Spreader_position = 1;
        }
        else
        {
          Spreader_position = 0;
        }
      }
    }

    //make 0 at the end of the new layer, so it can be written to again by the printing and identified as unwritten.
    smart_NL_F1_pos = 0;
    smart_NL_F2_pos = 0;



    //X-step_target
    NL_postemp = NL_X_target;
    NL_postemp *= X_steps_per_mm;
    NL_postemp /= 100;
    int NL_X_target_steps = NL_postemp;
    if (NL_X_target_steps == X_step_pos)
    {
    }
    else if (NL_X_target_steps > X_step_pos)
    {
      Set_X_direction(1);
    }
    else 
    {
      Set_X_direction(0);
    }


    G_target = micros() + G_delay;


    while (New_layer_ident == 20) // move X to set target
    {

      if (X_step_pos != NL_X_target_steps)
      {
        if (micros() - G_target < 10000000)
        {
          Step_X();
          G_target += G_delay;
        }
      }

      if (X_step_pos == NL_X_target_steps)
      {
        New_layer_ident = 30;
      }

    }
  }
}

void reset_spreader_possition()
{
  Spreader_position = 0;
}

































