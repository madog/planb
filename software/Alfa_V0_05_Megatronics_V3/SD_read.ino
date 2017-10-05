/*
The SD read tab is responsible for reading the Print file from the SD card and processing the txt format to data stored in the buffers for printing.
 It reads one charecter at a time, processing the line as it passes. Once the line is done or there is no more usable information in the line,
 the firmware will export the read line to the buffer for printing.
 */
const byte nozzlecount = 12; //amount of nozzles in cartridge


char temp_block[5]; //temporary storage for single characters
byte block_pos = 0; //where in the block is being read
byte block_pos_target; //where to stop on the X, Y and Z numerators (used after a comma)
byte line_pos = 0; //where in the line is being read (X,Y,Z, identifiers, nozzles, direction ie.) 
int temp_buffer[5]; //temporary line buffer
byte nozzle_history[14]; //what nozzles were on the previous line
byte nozzle_stated[14]; //what nozzles are being changed
byte line_code = 0; //detemines if a line is a nozzle code or a move code, 0 is undetermined, 1 is nozzle code, 2 is move code
boolean negative_block = 0; //determines if a block is positive (0) or negative (1)

//for line pos:
//0 is read first identifier letter
//1 is read numerators for G
//2 is read X-axis movement
//3 is read Y-axis movement
//4 is read Z-axis movement
//5 is read direction numerator
//6 is read toggle numerator
//11 is read N numererator;
//9 is end of line, no more useful info or violation in line

//Export_buffer:
//[0], 1001 = G1, 2000 = nozzle codes (G codes are += 1000, Nozzle codes are just 2000)


void read_sd_card()
{
  byte character;
  if (printFile.available())
  {
    character = printFile.read();  //read a new character
  }
  else
  {
    end_of_file = 1;
    write_to_temp_buffer();
    write_to_export_buffer();
  }


  //if a new line 
  if (end_of_file == 0)
  {
    if (character == 10) // new line, end of line
    {
      write_to_temp_buffer();
      write_to_export_buffer();
    }

    else if (character == 32) // Space, end of block
    {
      write_to_temp_buffer();
    }

    else
    {
      switch (line_pos)
      {
      case 0: //identifier, read the letter before the following number to see what kind of command (G, N, X, Y, Z, D, T or E) needs a number
        switch (character)
        {
        case 71: // G
          if (line_code == 0) //line_code undetermined? if not, double line_code, violation!
          {
            line_pos = 1;
            line_code = 2;
          }
          else 
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
          break;

        case 78: // N
          if (line_code == 0 || line_code == 1) //line_code undetermined or nozzle, if not, violation
          {
            line_pos = 11;
            line_code = 1;
          }
          else 
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
          break;

        case 88: // X
          line_pos = 2;
          break;

        case 89: // Y
          line_pos = 3;
          break;

        case 90: // Z
          line_pos = 4;
          break;

        case 68: // D
          if (line_code == 2) //line_code is Movement identified?
          {
            line_pos = 5;
          }
          else
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
          break;

        case 84: // T
          if (line_code == 1) //line_code is Nozzle identified?
          {
            line_pos = 6;
          }
          else
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
          break;

        case 69: // E
          line_pos = 7;
          break;

        case 70: // F
          line_pos = 8;
          break;

          //add exception for Feedrate (ignore but don't block code)

        default:
          line_pos = 9; //violation! stop reading (9 is unlinked)
          break;
        }
        break;

      case 1: //G numerator
        if (character >= 48 && character <= 57) // only if its a number
        {
          if (block_pos < 3) // For identifier numerators, the amount of numbers should never exceed 3
          {
            temp_block[block_pos] = character;
            block_pos ++;
          }
          else //else identifier is invalid, ignore line.
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
        }
        else if (character == 13)
        {          
        }
        else // if not,identifier is invalid, ignore line
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 11: //N numerator
        if (character >= 48 && character <= 57) // only if its a number
        {
          if (block_pos < 2) // For nozzle numerators, the amount of numbers should never exceed 2 (0-11)
          {
            temp_block[block_pos] = character;
            block_pos ++;
          }
          else //else identifier is invalid, ignore line.
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
        }
        else if (character == 13) //if new line, the code is still valid, else if used to prevent line_pos = 9;
        {          
        }
        else // if not,identifier is invalid, ignore line
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 2: //X numerator
        if (character == 45)
        {
          negative_block = 1;
        }
        else if (character >= 48 && character <= 57) // only if its a number or a point
        {
          if (block_pos < block_pos_target)
          {
            if (block_pos < 5) // For movement numerators, the amount of numbers should never exceed 5
            {
              temp_block[block_pos] = character;
              block_pos ++;
            }
            else //else identifier is invalid, ignore line.
            {
              line_pos = 9; //violation! stop reading (9 is unlinked)
            }
          }
        }
        else if (character == 46)
        {
          block_pos_target = block_pos + 2; //only 2 more characters should be read
        }
        else if (character == 13)
        {          
        }
        else
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 3: //Y numerator
        if (character == 45)
        {
          negative_block = 1;
        }
        else if (character >= 48 && character <= 57) // only if its a number or a point
        {
          if (block_pos < block_pos_target)
          {
            if (block_pos < 5) // For movement numerators, the amount of numbers should never exceed 5
            {
              temp_block[block_pos] = character;
              block_pos ++;
            }
            else //else identifier is invalid, ignore line.
            {
              line_pos = 9; //violation! stop reading (9 is unlinked)
            }
          }
        }
        else if (character == 46)
        {
          block_pos_target = block_pos + 2; //only 2 more characters should be read
        }
        else if (character == 13)
        {          
        }
        else
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 4: //Z-numerator
        if (character == 45)
        {
          negative_block = 1;
        }
        else if (character >= 48 && character <= 57) // only if its a number or a point
        {
          if (block_pos < block_pos_target)
          {
            if (block_pos < 5) // For movement numerators, the amount of numbers should never exceed 5
            {
              temp_block[block_pos] = character;
              block_pos ++;
            }
            else //else identifier is invalid, ignore line.
            {
              line_pos = 9; //violation! stop reading (9 is unlinked)
            }
          }
        }
        else if (character == 46)
        {
          block_pos_target = block_pos + 2; //only 2 more characters should be read
        }
        else if (character == 13)
        {          
        }
        else
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 5: //D-numerator
        if (character >= 48 && character <= 57) // only if its a number
        {
          if (block_pos < 1) // For direction numerators, the amount of numbers should never exceed 1 (0-1)
          {
            temp_block[block_pos] = character;
            block_pos ++;
          }
          else //else identifier is invalid, ignore line.
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
        }
        else if (character == 13)
        {          
        }
        else // if not,direction is invalid, ignore entire line
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;   

      case 6: //T_numerator
        if (character >= 48 && character <= 57) // only if its a number
        {
          if (block_pos < 1) // For toggle numerators, the amount of numbers should never exceed 1 (0-1)
          {
            temp_block[block_pos] = character;
            block_pos ++;
          }
          else //else identifier is invalid, ignore line.
          {
            line_pos = 9; //violation! stop reading (9 is unlinked)
          }
        }
        else if (character == 13)
        {          
        }
        else // if not, toggle is invalid, ignore entire line
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      case 7: //E numerator
        if (character == 45)
        {
          negative_block = 1;
        }
        else if (character >= 48 && character <= 57) // only if its a number or a point
        {
          if (block_pos < block_pos_target)
          {
            if (block_pos < 5) // For movement numerators, the amount of numbers should never exceed 5
            {
              temp_block[block_pos] = character;
              block_pos ++;
            }
            else //else identifier is invalid, ignore line.
            {
              line_pos = 9; //violation! stop reading (9 is unlinked)
            }
          }
        }
        else if (character == 46)
        {
          block_pos_target = block_pos + 2; //only 2 more characters should be read
        }
        else if (character == 13)
        {          
        }
        else
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;
        
        case 8: //F numerator //simple passthrough, no values, no linking
        if (character == 45)
        {
        }
        else if (character >= 48 && character <= 57) // only if its a number or a point
        {
        }
        else if (character == 46)
        {
        }
        else if (character == 13)
        {          
        }
        else
        {
          line_pos = 9; //violation! stop reading (9 is unlinked)
        }
        break;

      }
    }
  }
}




void write_to_export_buffer()
{
  clear_export_buffer();
  clear_nozzle_export();
  if (line_code == 1)
  {
    temp_buffer[0] = 2000; 
    for (int t = 2; t < nozzlecount + 2; t++)
    {
      bitWrite(nozzle_export, t, nozzle_history[t]);
    }
  }
  else if (line_code == 2)
  {
    clear_nozzle_history();
  }

  export_buffer[0] = temp_buffer[0];

  for (int t = 1; t < 5; t++)
  {
    if (temp_buffer[t] != -1)
    {
      export_buffer[t] = temp_buffer[t];
    }
  }

  New_line = 1;

  clear_line_data();
  clear_block_data();
  clear_nozzle_stated();
}





void write_to_temp_buffer()
{
  if ((line_pos >= 1 && line_pos <= 7) || line_pos == 11) //only export block if the line is still free of violations
  {
    switch (line_pos) //determine place in buffer
    {

    case 1: //G identifier
      {
        temp_buffer[0] = atoi(temp_block);
        if (line_code == 2)
        {
          temp_buffer[0] += 1000;
        }
      }
      break;

    case 11: //N identifier
      {
        int temp = atoi(temp_block);
        if (temp >= 0 && temp < nozzlecount)
        {
          nozzle_stated[temp] = 1; 
        }
        else
        {
          line_pos = 9;
        }
      }
      break;  

    case 2: //X identifier
      {
        if (block_pos_target == 10 && block_pos < 3) // no comma detected, few enough characters
        {
          temp_block[block_pos] = 48;
          block_pos ++;
          temp_block[block_pos] = 48;
          block_pos ++;
        }
        else if (block_pos_target != 10 && block_pos < block_pos_target)
        {
          int temppos = block_pos_target - block_pos;
          for (int p = 0; p < temppos; p++)
          {
            temp_block[block_pos] = 48;
            block_pos ++;
          }
        }
        else if (block_pos_target == block_pos)
        {
        }
        else
        {
          line_pos = 9;
        }

        if (line_pos != 9)
        {
          int temp = atoi(temp_block);
          if (negative_block == 1)
          {
            temp *= -1;
          }
          if (temp == -1)
          {
            temp = 0;
          }
          temp_buffer[1] = temp;

        }
      }
      break;

    case 3: //Y identifier
      {
        if (block_pos_target == 10 && block_pos < 3) // no comma detected, few enough characters
        {
          temp_block[block_pos] = 48;
          block_pos ++;
          temp_block[block_pos] = 48;
          block_pos ++;
        }
        else if (block_pos_target != 10 && block_pos < block_pos_target)
        {
          int temppos = block_pos_target - block_pos;
          for (int p = 0; p < temppos; p++)
          {
            temp_block[block_pos] = 48;
            block_pos ++;
          }
        }
        else if (block_pos_target == block_pos)
        {
        }
        else
        {
          line_pos = 9;
        }

        if (line_pos != 9)
        {
          int temp = atoi(temp_block);
          if (negative_block == 1)
          {
            temp *= -1;
          }
          if (temp == -1)
          {
            temp = 0;
          }
          temp_buffer[2] = temp;

        }
      }
      break;

    case 4: //Z identifier
      {
        if (block_pos_target == 10 && block_pos < 3) // no comma detected, few enough characters
        {
          temp_block[block_pos] = 48;
          block_pos ++;
          temp_block[block_pos] = 48;
          block_pos ++;
        }
        else if (block_pos_target != 10 && block_pos < block_pos_target)
        {
          int temppos = block_pos_target - block_pos;
          for (int p = 0; p < temppos; p++)
          {
            temp_block[block_pos] = 48;
            block_pos ++;
          }
        }
        else if (block_pos_target == block_pos)
        {
        }
        else
        {
          line_pos = 9;
        }

        if (line_pos != 9)
        {
          int temp = atoi(temp_block);
          if (negative_block == 1)
          {
            temp *= -1;
          }
          if (temp == -1)
          {
            temp = 0;
          }
          temp_buffer[3] = temp;

        }
      }
      break;

    case 5: //Direction identifier
      {
        int temp = atoi(temp_block);
        if (temp == 0)
        {
          temp_buffer[0] += 1; //make G1 (1001) +1, so 1002. 1002 is G1 with D0
        }
        else if (temp == 1)
        {
          temp_buffer[0] += 2; //make G1 (1001) +2, so 1003. 1003 is G1 with D1
        }
        else
        {
          line_pos = 9;
        }
      }
      break;

    case 6: //toggle identifier
      {
        //pack nozzle values in temp buffer
        //define if toggle on or toggle of
        int tempt = atoi(temp_block);
        if (tempt == 0 || tempt == 1)
        {
          for (int t = 2; t < nozzlecount + 2; t++)
          {
            if (nozzle_stated[t-2] == 1)
            {
              //bitWrite(nozzle_export, t, tempt);
              if (tempt == 1)
              {
                nozzle_history[t] = 1;
              }
              else
              {
                nozzle_history[t] = 0;
              }
            }         
            else
            {
              //bitWrite(nozzle_export, t, nozzle_history[t]);
            }     
          }
        }
        else
        {
          line_pos = 9;
        }
      }
      break;

    case 7: //E identifier
      {
        if (block_pos_target == 10 && block_pos < 3) // no comma detected, few enough characters
        {
          temp_block[block_pos] = 48;
          block_pos ++;
          temp_block[block_pos] = 48;
          block_pos ++;
        }
        else if (block_pos_target != 10 && block_pos < block_pos_target)
        {
          int temppos = block_pos_target - block_pos;
          for (int p = 0; p < temppos; p++)
          {
            temp_block[block_pos] = 48;
            block_pos ++;
          }
        }
        else if (block_pos_target == block_pos)
        {
        }
        else
        {
          line_pos = 9;
        }

        if (line_pos != 9)
        {
          int temp = atoi(temp_block);
          if (negative_block == 1)
          {
            temp *= -1;
          }
          if (temp == -1)
          {
            temp = 0;
          }
          temp_buffer[4] = temp;

        }
      }
      break;
    }
  }
  clear_block_data();
}







void clear_nozzle_history()
{
  for (int n = 0; n < nozzlecount; n++)
  {
    nozzle_history[n] = 0;
  }
}

void clear_nozzle_stated()
{
  for (int n = 0; n < nozzlecount; n++)
  {
    nozzle_stated[n] = 0; 
  }
}

void clear_nozzle_export()
{

  nozzle_export = 0;

}

void clear_block_data()
{
  block_pos = 0;
  line_pos = 0;
  negative_block = 0;
  block_pos_target = 10;
  for (int n = 0; n < 5; n++)
  {
    temp_block[n] = 0;
  }
}

void clear_line_data()
{
  line_pos = 0;
  line_code = 0;
  for (int n = 0; n < 6; n++)
  {
    temp_buffer[n] = -1;
  }
}

void clear_export_buffer()
{
  for (int t = 0; t < 5; t++)
  {
    export_buffer[t] = -1;
  }
}
//if next line or Sd card depleted, export code
//Export to: command, X, Y, Z, F and 2 bytes for all 12 nozzles.

//A byte to determine if I am reading at all or if I have reached the effective end of a line
//A byte to keep track of what kind of character I am reading
//Go to the place I am expecting
//Keep track of everything I have read in this cluster so far and store in temp value
//N identifiers can be stacked
//An G is never followed by an N
//Once the end of one cluster (Identifier + coordinates ie.) export to temp buffer, then clear temp value.
//Keep track of previous inkjet states to succesfully export a new line
//Once the end of a line has been reached, Write temp buffer to export buffer 
//Mark a boolean to tell printing a new line is ready for importing, printing will make this value 0 again.
//(Printing software will take values from the export buffer and put them in the print buffer)
//Clear temp buffer and all temp values
//All one character at a time
//SD_read is responsible for knowing if the SD still has more txt. If it runs out, SD_read should know and tell Printing, not the other way around
























































