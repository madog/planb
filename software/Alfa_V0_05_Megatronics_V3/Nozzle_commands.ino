/*
The Nozzle commands tab is responsible for all fuctions regarding the nozzles. It determines where what nozzles are triggered. 
 
 The nozzle set function determines what nozzles are on and off when the nozzle trigger command is given
 
 The nozzle trigger command triggers all nozzles as fast as possible.
 
 */
boolean nozzle_active[12];

void nozzle_trigger()
{  
  //nozzle trigger for CD4067
  //check which nozzles are on,
  for (byte n = 0; n < 12; n++)
  {
    if (nozzle_active[n] == 1)
    {
      //trigger that nozzle, based on value
      //portL, 0-3,
      byte ntemp = n + 1;

      // turn on nozzle n
      PORTL |= ntemp;

      //wait nozzle printing time
      delayMicroseconds(5);

      //turn off nozzle n
      PORTL &= B11110000;
    }
  }
}

void nozzle_single_trigger()
{ 
  for (byte n = 0; n < 12; n++)
  {
    if (n == printing_single_nozzle_print_nozzle)
    {
      //trigger that nozzle, based on value
      //portL, 0-3,
      byte ntemp = n + 1;

      // turn on nozzle n
      PORTL |= ntemp;

      //wait nozzle printing time
      delayMicroseconds(5);

      //turn off nozzle n
      PORTL &= B11110000;
    }
  }
} 

void nozzle_set(word nozzle_state)
{
  //set new nozzle targets
  byte ntemp;
  for (byte n = 0; n < 12; n++)
  {
    ntemp = n + 2;
    nozzle_active[n] = bitRead(nozzle_state, ntemp);
  }
  //(REMEMBER, nozzles in buffer go from 2 to 13, not 0 to 11)   
}







