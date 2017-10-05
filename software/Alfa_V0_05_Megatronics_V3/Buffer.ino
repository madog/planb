/*
The buffer tab is used to keep track of the buffer. It holds the number of slots in the buffer, it keeps track of it's current possition and when a command is given, it steps 
 to the next slot in the buffer. When the buffer receives an end of file, it will also tell when to stop printing (once the buffer has depleted). It has several outputs
 - slowdown is a command that is issued when the buffer is running out (due too high read speeds and not enough time to replenish) the printer can use this command to slow down
 the print to give the buffer more time to replenish code. In normal use, this command should never be used, functions are fast enough to replenish one line every inkjet 
 deposition at 100mm/s.
 - buffer read variables. These variables are defined in the main block. They span all blocks and depending on the commands given return the current line in the buffer, or the
 amount n offset if the line in the buffer.
 */

const int buffer_size = 100; //size of the buffer, Warning, each buffer slot takes 12 bytes of RAM.
const int slowdown_safety = 5; //the amount of buffer slots there must be between write and read, if smaller, then the firmware will issue a slowdown
const int slowdown_return = 10; //the amount of buffer slots there must be between write and read before slowdown is deactivated. must be between slowdown_safety and buffer_size
const int max_offset = 20; //the maximum offset allowed in the look ahead. If exceeded, buffer will stop looking to the next next line and issue a warning



int buffer_coordinate_slot[5][buffer_size];
word buffer_nozzle_slot[buffer_size];

int read_line = 0;
int write_line = 0;
byte read_offset = 0;


void buffer_update() //update buffer, has it's own safeties, can be used without checking without problems.
{
  if (empty_slots > 0)
  {
    if (end_of_file == 0)
    {
      read_sd_card();
    }
    if (New_line == 1)
    {
      New_line = 0;

      for (int w = 0; w < 5; w++)
      {
        buffer_coordinate_slot[w][write_line] = export_buffer[w];
      }
      buffer_nozzle_slot[write_line] = nozzle_export;

      write_line ++;
      if (write_line >= buffer_size)
      {
        write_line = 0;
      }

      empty_slots --;
    }
  }
}



void buffer_next() //go to the next slot in the buffer
{
  look_ahead_failure = 0;
  read_line++;
  if (read_line >= buffer_size)
  {
    read_line = 0;
  }

  for (int w = 0; w < 5; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }
  buffer_nozzle_read = buffer_nozzle_slot[read_line];

  if (end_of_file == 0)
  {
    empty_slots ++;
  }
  else 
  {
    if (read_line == write_line)
    {
      end_of_print = 1;
    }  
  }
  read_offset = 0;
}



void buffer_look_ahead() //looks ahead one slot to look for additional coordinates, can be stacked
{
  if (read_offset < max_offset)
  { 
    read_offset ++;
  }
  else
  {
    look_ahead_failure = 1;
  }

  int readtemp = read_line;
  readtemp += read_offset;

  if (readtemp >= buffer_size)
  {
    readtemp = 0;
  }

  if (end_of_file == 1 && readtemp == write_line)
  {
    look_ahead_failure = 1;
    read_offset--; //step back one step
    readtemp = read_line;
    readtemp += read_offset;

    if (readtemp >= buffer_size)
    {
      readtemp = 0;
    }

  }
  else
  {
    for (int w = 0; w < 5; w++)
    {
      buffer_coordinate_read[w] = buffer_coordinate_slot[w][readtemp];
    }
    buffer_nozzle_read = buffer_nozzle_slot[readtemp];
  }    
}



void buffer_current() //go back to current buffer slot
{
  look_ahead_failure = 0;
  read_offset = 0;
  for (int w = 0; w < 5; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }
  buffer_nozzle_read = buffer_nozzle_slot[read_line];
}



void buffer_fill() //fills entire buffer (used before printing, not any other place!)
{
  write_line = 0;
  read_line = 0;
  empty_slots = 0;
  read_offset = 0;
  while (write_line < buffer_size && end_of_file == 0)
  {
    read_sd_card();
    if (New_line == 1)
    {
      New_line = 0;

      for (int w = 0; w < 5; w++)
      {
        buffer_coordinate_slot[w][write_line] = export_buffer[w];
      }
      buffer_nozzle_slot[write_line] = nozzle_export;

      write_line++;
    }
  }
  if (write_line >= buffer_size)
  {
    write_line = 0;
  }
  empty_slots = 0;

  //put first command in read line
  for (int w = 0; w < 5; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }
  buffer_nozzle_read = buffer_nozzle_slot[read_line];
}




