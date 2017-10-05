/*
Motor commands carries all procedures to make the motors move. It also keeps track of the position of all motors. Axis can also be inverted here. Motor directions, enabling motors
and making steps should only be done by calling the functions, not manually, otherwise it will lose steps or move in the wrong direction.
*/

// motor settings
const boolean invert_X = 0;
const boolean invert_Y = 1;
const boolean invert_S = 1;
const boolean invert_F1 = 0;
const boolean invert_F2 = 1;
const boolean invert_B = 1;

// variables
boolean X_enabled = 0;
boolean Y_enabled = 0;
boolean S_enabled = 0;
boolean F1_enabled = 0;
boolean F2_enabled = 0;
boolean B_enabled = 0;

boolean X_dir = 0;
boolean Y_dir = 0;
boolean S_dir = 0;
boolean F1_dir = 0;
boolean F2_dir = 0;
boolean B_dir = 0;

//enable / disable --------------------------------------------------------------------------
void Enable_X()
{
  pinMode(X_motor_ena, OUTPUT);
  X_enabled = 1;
}
void Disable_X()
{
  pinMode(X_motor_ena, INPUT);
  X_enabled = 0;
}

void Enable_Y()
{
  pinMode(Y_motor_ena, OUTPUT);
  Y_enabled = 1;
}
void Disable_Y()
{
  pinMode(Y_motor_ena, INPUT);
  Y_enabled = 0;
}

void Enable_S()
{
  pinMode(S_motor_ena, OUTPUT);
  S_enabled = 1;
}
void Disable_S()
{
  pinMode(S_motor_ena, INPUT);
  S_enabled = 0;
}

void Enable_F1()
{
  pinMode(F1_motor_ena, OUTPUT);
  F1_enabled = 1;
}
void Disable_F1()
{
  pinMode(F1_motor_ena, INPUT);
  F1_enabled = 0;
}

void Enable_F2()
{
  pinMode(F2_motor_ena, OUTPUT);
  F2_enabled = 1;
}
void Disable_F2()
{
  pinMode(F2_motor_ena, INPUT);
  F2_enabled = 0;
}

void Enable_B()
{
  pinMode(B_motor_ena, OUTPUT);
  B_enabled = 1;
}
void Disable_B()
{
  pinMode(B_motor_ena, INPUT);
  B_enabled = 0;
}

void Enable_All()
{
  pinMode(X_motor_ena, OUTPUT);
  pinMode(Y_motor_ena, OUTPUT);
  pinMode(S_motor_ena, OUTPUT);
  pinMode(F1_motor_ena, OUTPUT);
  pinMode(F2_motor_ena, OUTPUT);
  pinMode(B_motor_ena, OUTPUT);
  X_enabled = 1;
  Y_enabled = 1;
  S_enabled = 1;
  F1_enabled = 1;
  F2_enabled = 1;
  B_enabled = 1;  
}

void Disable_All()
{
  pinMode(X_motor_ena, INPUT);
  pinMode(Y_motor_ena, INPUT);
  pinMode(S_motor_ena, INPUT);
  pinMode(F1_motor_ena, INPUT);
  pinMode(F2_motor_ena, INPUT);
  pinMode(B_motor_ena, INPUT);
  X_enabled = 0;
  Y_enabled = 0;
  S_enabled = 0;
  F1_enabled = 0;
  F2_enabled = 0;
  B_enabled = 0;  
}

//Set Direction commands ---------------------------------------------------------------------
void Set_X_direction(boolean tempdir)
{
  switch (invert_X)
  {
  case 0:
    digitalWrite(X_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(X_motor_dir, !tempdir);
    break;
  }
  X_dir = tempdir;
}

void Set_Y_direction(boolean tempdir)
{
  switch (invert_Y)
  {
  case 0:
    digitalWrite(Y_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(Y_motor_dir, !tempdir);
    break;
  }

  Y_dir = tempdir;
}

void Set_S_direction(boolean tempdir)
{
  switch (invert_S)
  {
  case 0:
    digitalWrite(S_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(S_motor_dir, !tempdir);
    break;
  }
  S_dir = tempdir;
}

void Set_F1_direction(boolean tempdir)
{
  switch (invert_F1)
  {
  case 0:
    digitalWrite(F1_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(F1_motor_dir, !tempdir);
    break;
  }
  F1_dir = tempdir;
}

void Set_F2_direction(boolean tempdir)
{
  switch (invert_F2)
  {
  case 0:
    digitalWrite(F2_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(F2_motor_dir, !tempdir);
    break;
  }
  F2_dir = tempdir;
}

void Set_B_direction(boolean tempdir)
{
  switch (invert_B)
  {
  case 0:
    digitalWrite(B_motor_dir, tempdir);
    break;
  case 1:
    digitalWrite(B_motor_dir, !tempdir);
    break;
  }
  B_dir = tempdir;
}

//simple step commands -----------------------------------------------------------------------
//!!!simple step is not protected for collisions
void Step_X()
{
  if (X_enabled == 1)
  {
    digitalWrite(X_motor_step, 1);
    digitalWrite(X_motor_step, 0);
    if (GHomed == 1)
    {
      if (X_dir == 1)
      {
        X_step_pos++;
      }
      else if (X_dir == 0)
      {
        X_step_pos--;
      }
    }
  }
}

void Step_Y()
{
  if (Y_enabled == 1)
  {
    digitalWrite(Y_motor_step, 1);
    digitalWrite(Y_motor_step, 0);
    if (GHomed == 1)
    {
      if (Y_dir == 1)
      {
        Y_step_pos++;
        X_step_pos++;
      }
      else if (Y_dir == 0)
      {
        Y_step_pos--;
        X_step_pos--;
      }
    }
  }
}

void Step_S()
{
  if (S_enabled == 1)
  {
    digitalWrite(S_motor_step, 1);
    digitalWrite(S_motor_step, 0);
    if (SHomed == 1)
    {
      if (S_dir == 1)
      {
        S_step_pos++;
      }
      else if (S_dir == 0)
      {
        S_step_pos--;
      }
    }
  }
}

void Step_F1()
{
  if (F1_enabled == 1)
  {
    digitalWrite(F1_motor_step, 1);
    digitalWrite(F1_motor_step, 0);
    if (F1Homed == 1)
    {
      if (F1_dir == 1)
      {
        F1_step_pos++;
      }
      else if (F1_dir == 0)
      {
        F1_step_pos--;
      }
    }
  }
}

void Step_F2()
{
  if (F2_enabled == 1)
  {
    digitalWrite(F2_motor_step, 1);
    digitalWrite(F2_motor_step, 0);
    if (F2Homed == 1)
    {
      if (F2_dir == 1)
      {
        F2_step_pos++;
      }
      else if (F2_dir == 0)
      {
        F2_step_pos--;
      }
    }
  }
}

void Step_B()
{
  if (B_enabled == 1)
  {
    digitalWrite(B_motor_step, 1);
    digitalWrite(B_motor_step, 0);
    if (BHomed == 1)
    {
      if (B_dir == 1)
      {
        B_step_pos++;
      }
      else if (B_dir == 0)
      {
        B_step_pos--;
      }
    }
  }
}




