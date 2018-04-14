/*
The heating tab is responsible for all heating related functions. It sets and checks temperature on command.
 */

int build_sensor_temperature;
int build_set_temperature;

// thermistor tabel acording to reprap 100k thermistor. It is not a 100% match, but for the purpose it is close enough.

// EPCOS 100K Thermistor (B57540G0104F000)
// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 4066
// max adc: 1023
#define NUMTEMPS 20
short temptable[NUMTEMPS][2] = {
  {
    1, 841    }
  ,
  {
    54, 255    }
  ,
  {
    107, 209    }
  ,
  {
    160, 184    }
  ,
  {
    213, 166    }
  ,
  {
    266, 153    }
  ,
  {
    319, 142    }
  ,
  {
    372, 132    }
  ,
  {
    425, 124    }
  ,
  {
    478, 116    }
  ,
  {
    531, 108    }
  ,
  {
    584, 101    }
  ,
  {
    637, 93    }
  ,
  {
    690, 86    }
  ,
  {
    743, 78    }
  ,
  {
    796, 70    }
  ,
  {
    849, 61    }
  ,
  {
    902, 50    }
  ,
  {
    955, 34    }
  ,
  {
    1008, 3    }
};




void read_temperature()
{
  //read buildbox temperature
  long temp_temp = analogRead(B_sens);

  //is it perfect match? note temperature, 
  boolean match = 0;
  for (int t = 0; t < NUMTEMPS; t++)
  {
    if (temptable[t][0] == temp_temp)
    {
      match = 1;
      build_sensor_temperature = temptable[t][1];
    }
  }

  //if not perfect match, rescan and find 2 closet values
  int temp_up_pos;
  int temp_down_pos;
  if (match == 0)
  {
    for (int t = 0; t < NUMTEMPS; t++)
    {
      if (temptable[t][0] > temp_temp)
      {
        temp_up_pos = t;
      }
    }
    temp_down_pos = temp_up_pos - 1;

    //linearize from two values, write prediction of the temperature down.
    build_sensor_temperature = map(temp_temp, temptable[temp_down_pos][0], temptable[temp_up_pos][0], temptable[temp_down_pos][1], temptable[temp_up_pos][1]);

    //depending on how much it is in the middle, subtract a portion of the difference from the value.
  }  


}

int update_temperature()
{
  read_temperature();

  //read sensors, compare to set values and turn on or off heaters
  //if temperature is out of range, turn off heaters.
  if (build_sensor_temperature < 0 && build_sensor_temperature > 100)
  {
    digitalWrite(Bed_heater, 0);
    digitalWrite(Feed1_heater, 0);
    digitalWrite(Feed2_heater, 0);
  }
  else if (build_sensor_temperature < build_set_temperature)
  {
    digitalWrite(Bed_heater, 1);
    digitalWrite(Feed1_heater, 1);
    digitalWrite(Feed2_heater, 1);
  }
  else
  {
    digitalWrite(Bed_heater, 0);
    digitalWrite(Feed1_heater, 0);
    digitalWrite(Feed2_heater, 0);
  }

  //return build_sensor_temperature;
}

void set_build_temperature(word b_temperature)
{
  //set the build temperature value to the set amount
  if (b_temperature >= 0 && b_temperature < 100)
  {
    build_set_temperature = b_temperature;
    update_temperature();
  }
}





