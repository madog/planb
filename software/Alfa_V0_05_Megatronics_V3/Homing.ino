/*
The homing tab is resonsible for all homing motions. Homing is a moving command but has its own tab because it moves without keeping track of position. It homes all axis and 
 determines what coordinates tha axis are at.
 */


//All homing commands behave the same
//Once executed, move said axis toward the endstop at a given constant speed
//When the endstops is activated, reverse direction and move a given amount away from the endstop
//Then move the axis toward the endstop again at a slower speed
//When the endstop is triggered again, stop the motor and mark that location with the coordinates of that location.

//Gantry movement is split in two. First the Y-axis with the carrier homes. ONLY after that the X-axis homes.
//Y-axis is a hybrid movement, it shares some of it's movement with the X-axis. 

//All homing commands are interruptive. They interrupt the normal data processing stream. If important processes are running, homing commands should not be activated.





const int S_home_speed = 1000; //in microseconds of delay
const int S_home_backstep = 100; //in steps
const int Y_extra_movement = 400; //in steps, to get out of the way of the spreader
const int G_home_speed = 1000;
const int G_home_backstep = 50;
const int F1_home_speed = 500;
const int F1_home_backstep = 500;
const int F2_home_speed = 500;
const int F2_home_backstep = 500;
const int B_home_speed = 500;
const int B_home_backstep = 500;

//offsets for now entered in number of steps. negative is away from the endstops
const int S_offset = -2000;
const int X_offset = -800; //old value -4600 for center
const int Y_offset = -1200; //old value -5400 for center
const int F1_offset = 0;
const int F2_offset = 0;
const int B_offset = 0;




void Home_gantry()
{
  X_step_target = X_offset;
  Y_step_target = Y_offset;
  byte G_home_tracker = 0;
  int readcounter = 0;
  const int readtarget = 100;
  Enable_X();
  Enable_Y();
  Set_X_direction(0);

  X_stop();


  while (G_home_tracker == 0)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_X();
    readcounter++;
    delayMicroseconds(G_home_speed);

    if (digitalRead(X_endstop) == 1)
    {
      G_home_tracker = 10;
      Set_X_direction(1);
    }
    if (switchLeft && switchRight)
    {
      G_home_tracker = 50;
    }
  }
  if (G_home_tracker == 10)
  {
    for(int s = 0; s < G_home_backstep; s++)
    {
      Step_X();
      delayMicroseconds(G_home_speed);
    }
  }

  Set_X_direction(0);
  int G_speed_temp = G_home_speed * 5;

  while (G_home_tracker == 10)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_X();
    readcounter++;
    delayMicroseconds(G_speed_temp);

    if (digitalRead(X_endstop) == 1)
    {
      G_home_tracker = 20;
    }
    if (switchLeft && switchRight)
    {
      G_home_tracker = 50;
    }
  }

  Set_Y_direction(0);
  Set_X_direction(1);

  while (G_home_tracker == 20)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_Y();
    Step_X();
    readcounter++;
    delayMicroseconds(G_home_speed);

    if (digitalRead(Y_endstop) == 1)
    {
      G_home_tracker = 30;
      Set_Y_direction(1);
      Set_X_direction(0);
    }
    if (switchLeft && switchRight)
    {
      G_home_tracker = 50;
    }
  }
  if (G_home_tracker == 30)
  {
    for(int s = 0; s < G_home_backstep; s++)
    {
      Step_Y();
      Step_X();
      delayMicroseconds(G_home_speed);
    }
  }

  Set_Y_direction(0);
  Set_X_direction(1);

  while (G_home_tracker == 30)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_Y();
    Step_X();
    readcounter++;
    delayMicroseconds(G_speed_temp);

    if (digitalRead(Y_endstop) == 1)
    {
      G_home_tracker = 40;
      GHomed = 1;
      X_step_pos = X_offset;
      Y_step_pos = Y_offset;
    }
    if (switchLeft && switchRight)
    {
      G_home_tracker = 50;
    }
  }


  if (G_home_tracker == 40)
  {
    Set_Y_direction(1);
    Set_X_direction(0);
    for (int g = 0; g < Y_extra_movement; g++)
    {
      Step_Y();
      Step_X();
      delayMicroseconds(G_home_speed);
    }
  }

}



void Home_S()
{
  reset_spreader_possition();
  byte S_home_tracker = 0;
  int readcounter = 0;
  const int readtarget = 100;
  Enable_S();
  Set_S_direction(0);


  while (S_home_tracker == 0)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_S();
    readcounter++;
    delayMicroseconds(S_home_speed);

    if (digitalRead(S_endstop) == 1)
    {
      S_home_tracker = 10;
      Set_S_direction(1);
    }
    if (switchLeft && switchRight)
    {
      S_home_tracker = 20;
    }
  }
  if (S_home_tracker == 10)
  {
    for(int s = 0; s < S_home_backstep; s++)
    {
      Step_S();
      delayMicroseconds(S_home_speed);
    }
  }

  Set_S_direction(0);
  int S_speed_temp = S_home_speed * 5;

  while (S_home_tracker == 10)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_S();
    readcounter++;
    delayMicroseconds(S_speed_temp);

    if (digitalRead(S_endstop) == 1)
    {
      S_home_tracker = 20;
      SHomed = 1;
      S_step_pos = S_offset;
    }
    if (switchLeft && switchRight)
    {
      S_home_tracker = 20;
    }
  }

}



void Home_F1()
{
  byte F1_home_tracker = 0;
  int readcounter = 0;
  const int readtarget = 100;
  Enable_F1();
  Set_F1_direction(0);


  while (F1_home_tracker == 0)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_F1();
    readcounter++;
    delayMicroseconds(F1_home_speed);

    if (digitalRead(F1_endstop) == 1)
    {
      F1_home_tracker = 10;
      Set_F1_direction(1);
    }
    if (switchLeft && switchRight)
    {
      F1_home_tracker = 20;
    }
  }
  if (F1_home_tracker == 10)
  {
    for(int s = 0; s < F1_home_backstep; s++)
    {
      Step_F1();
      delayMicroseconds(F1_home_speed);
    }
  }

  Set_F1_direction(0);
  int F1_speed_temp = F1_home_speed * 5;

  while (F1_home_tracker == 10)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_F1();
    readcounter++;
    delayMicroseconds(F1_speed_temp);

    if (digitalRead(F1_endstop) == 1)
    {
      F1_home_tracker = 20;
      F1Homed = 1;
      F1_step_pos = F1_offset;
    }
    if (switchLeft && switchRight)
    {
      F1_home_tracker = 20;
    }
  }
}



void Home_F2()
{
  byte F2_home_tracker = 0;
  int readcounter = 0;
  const int readtarget = 100;
  Enable_F2();
  Set_F2_direction(0);


  while (F2_home_tracker == 0)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_F2();
    readcounter++;
    delayMicroseconds(F2_home_speed);

    if (digitalRead(F2_endstop) == 1)
    {
      F2_home_tracker = 10;
      Set_F2_direction(1);
    }
    if (switchLeft && switchRight)
    {
      F2_home_tracker = 20;
    }
  }
  if (F2_home_tracker == 10)
  {
    for(int s = 0; s < F2_home_backstep; s++)
    {
      Step_F2();
      delayMicroseconds(F2_home_speed);
    }
  }

  Set_F2_direction(0);
  int F2_speed_temp = F2_home_speed * 5;

  while (F2_home_tracker == 10)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_F2();
    readcounter++;
    delayMicroseconds(F2_speed_temp);

    if (digitalRead(F2_endstop) == 1)
    {
      F2_home_tracker = 20;
      F2Homed = 1;
      F2_step_pos = F2_offset;
    }
    if (switchLeft && switchRight)
    {
      F2_home_tracker = 20;
    }
  }
}



void Home_B()
{
  byte B_home_tracker = 0;
  int readcounter = 0;
  const int readtarget = 100;
  Enable_B();
  Set_B_direction(0);


  while (B_home_tracker == 0)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
      readcounter = 0;
    }

    Step_B();
    readcounter++;
    delayMicroseconds(B_home_speed);

    if (digitalRead(B_endstop) == 1)
    {
      B_home_tracker = 10;
      Set_B_direction(1);
    }
    if (switchLeft && switchRight)
    {
      B_home_tracker = 20;
    }
  }
  if (B_home_tracker == 10)
  {
    for(int s = 0; s < B_home_backstep; s++)
    {
      Step_B();
      delayMicroseconds(B_home_speed);
    }
  }

  Set_B_direction(0);
  int B_speed_temp = B_home_speed * 5;

  while (B_home_tracker == 10)
  {
    if (readcounter > readtarget)
    {
      inputs_update();
    }

    Step_B();
    readcounter++;
    delayMicroseconds(B_speed_temp);

    if (digitalRead(B_endstop) == 1)
    {
      B_home_tracker = 20;
      BHomed = 1;
      B_step_pos = B_offset;
    }
    if (switchLeft && switchRight)
    {
      B_home_tracker = 20;
    }
  }
}




