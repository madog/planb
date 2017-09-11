/*
This tab is responsible for all button and encoder inputs. These is a single command specified here that upon addressing reads all button values and determines the encoder position
*/

int encoder_dir_counter = 0;

//start of shiftin read block----------------------------------------
void inputs_update()
{
  boolean bitVal;

  /* Trigger a parallel Load to latch the state of the data lines,
   */
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);

  /* Loop to read each bit value from the serial out line
   * of the SN74HC165N.
   */

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchMenu3 = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW); 

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchMenu2 = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW); 

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchMenu1 = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchUp = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW); 

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchRight = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW); 

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchEnter = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW); 

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchDown = bitVal;
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  bitVal = digitalRead(dataPin);
  if (bitVal == 1) {
    bitVal = 0;
  } 
  else {
    bitVal = 1;
  }
  switchLeft = bitVal; 
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  //Encoder update
  //first requirements
  
  //this block reads the encoder. It first determines if something has changed by comparing the current value with the history. Then it determines where the encoder
  //was last time (with the "encoder_dir_counter) and compares the new value with the possibilities. It the dic_counter reaches a prediced end (3+ or -3-) it will 
  //either increase or decrease "Encoderdirection" by one. The menu tab resets this value to 0 once it has used a usable value (<0 or >0).
  if  (Encoder1history != digitalRead(encoderPin1) || Encoder2history != digitalRead(encoderPin2))
  {
    if (encoder_dir_counter == 0)
    {
      if (digitalRead(encoderPin1) == 0)
      {
        encoder_dir_counter = 1;
      }
      else if (digitalRead(encoderPin2) == 0)
      {
        encoder_dir_counter = -1;
      }
    }

    //counter clockwise direction
    else if (encoder_dir_counter == -1)
    {
      if (digitalRead(encoderPin1) == 0)
      {
        encoder_dir_counter = -2;
      }
      else if(digitalRead(encoderPin2) == 1)
      {
        encoder_dir_counter = 0;
      }
    }

    else if (encoder_dir_counter == -2)
    {
      if (digitalRead(encoderPin2) == 1)
      {
        encoder_dir_counter = -3;
      }
      else if(digitalRead(encoderPin1) == 1)
      {
        encoder_dir_counter = -1;
      }
    }  

    else if (encoder_dir_counter == -3)
    {
      if (digitalRead(encoderPin1) == 1)
      {
        encoder_dir_counter = 0;
        Encoderdirection --;
      }
      else if(digitalRead(encoderPin2) == 0)
      {
        encoder_dir_counter = -2;
      }
    }
    //clockwise direction
    else if (encoder_dir_counter == 1)
    {
      if (digitalRead(encoderPin2) == 0)
      {
        encoder_dir_counter = 2;
      }
      else if(digitalRead(encoderPin1) == 1)
      {
        encoder_dir_counter = 0;
      }
    }

    else if (encoder_dir_counter == 2)
    {
      if (digitalRead(encoderPin1) == 1)
      {
        encoder_dir_counter = 3;
      }
      else if(digitalRead(encoderPin2) == 1)
      {
        encoder_dir_counter = 1;
      }
    }  

    else if (encoder_dir_counter == 3)
    {
      if (digitalRead(encoderPin2) == 1)
      {
        encoder_dir_counter = 0;
        Encoderdirection ++;
      }
      else if(digitalRead(encoderPin1) == 0)
      {
        encoder_dir_counter = 2;
      }
    }




  }

  Encoder1history = digitalRead(encoderPin1);
  Encoder2history = digitalRead(encoderPin2);


}  


//end of shift in read block-----------------------------------





