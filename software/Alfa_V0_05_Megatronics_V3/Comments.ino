/*
This firmware was writen specifically for Plan B, A powder/inkjet printer designed and built by Yvo de Haas (me).

The firmware is by far a work of art, I am mainly an engineer, partially an electrotechnician but I am not really a programmer. I did my best, implementing everything I have learned
from my past experience writing code for Focus, but I still retain that there are a lot of people out there that can do a better, more elegant job. Nevertheless, I hope this firmware
helps the first generation of powder/inkjet printers become reality. It really is a unique technique that has a lot of unique properties.

Currently the firmware is a work in progress, no bugs have been documented since the firmware is still under construction.

*/



/*
To do list

Everything that still needs to be done in Plan B firmware. 

-add acceleration to the motion of the firmware. Is currently not important but when later the speed
increases the movement needs a speedup to prevent missing steps.
(edit, for the start and stop constant movement acceleration has been added as a interuptive while loop)

-Add heating to the firmware. This includes:
o Add a check heater based on a millis() timer. Checks a set value to a measured value ever 10 seconds and toggles the heater on or off
o Add a read temperature block for several sensors. (test with the hardware)
o Add a set heat menu tab to the menu, allowing to preheat the hopper << function still broken but in place, properly test
o (Add a safety to the print to ask to continue if the build piston is not hot enough)
o Add a check heat every layer of the printing
o Add a reheating block with a timer at the end of the print, counting down from for example 1 hour to 0 with a time ... of ... and add speaker to tell time is up.


*/



/*
Things about the buffer tab:

- The buffer tab is an aditional layer between the SD read and the print that temporarily stores the values in the RAM memory of the arduino. It is necessary to keep printing
  when the speed is high. In Focus (the predecesor of Plan B) this was handles by the printing command itself and it caused a lot of problems. Buffer is responsible for buffering 
  values, reading the SD card and telling when to stop printing. 

Things about the Movement tab:

- The gantry has NO ACCELERATION. The function has not been thought out yet. It will be added in later versions, but for now, keep the speeds below 100mm/s to prevent missing steps.
- Gantry speed has a limited speed. This is defined in the movement tab, G_max_speed. If a command for a higher speed is given, it will be automatically reduced to the maximum 
  value.
- due to the composite drive of the X/Y, diagonal movements are not ideal. Where both Y and X move in the possitive direction, X-motor is stationary. Where they move in 
  opposite directions, X has to move faster than Y. This means that in some cases the X_motor needs to run faster than it is capable of. This only happens at high speeds (above
  100mm/s). At these speeds, other problems start to occur too. Mainly that the X and the Y don't arrive at the same time. This problem is not yet fixed and won't be fixed in this
  version and maybe not even in some other future versions.
- BE VERY CAREFUL when tweaking "constant_movement_X_acceleration" (main), "X_print_speed" (main) and "X_acceleration_distance" (printing). These values are dependent on eachother
  When modifying one, recalculate the others using the following formula: 
  X_acceleration_distance = (50 * constant_movement_X_acceleration * (X_print_speed / constant_movement_X_acceleration)^2) + 10
  this makes sure the constant movement acceleration is at it's full speed when the first nozzle target is reached. Not doing this right will mean the printer ceizing.
  
Things about the Printing tab:

- Due to some mysterious thing in the SD library, a file has to be completely read before it is closed if it is to be read properly a second time. For most procedures this is no 
  problem, since they read the whole file anyway (estimate, 3D print). However, for the procedures that only read a file partially (PCB print, pause), the file needs to be 
  depleted manually. This takes some extra time afterward, but is necessary if the file has to be opened another time.
- The file to print is stored on an SD card. This file currently has to be stored as "print.txt". I still lack the skills to both read the SD card, scanning for available files
  and opening a Gcode file. For now, bear with this. No fix is planned yet, I fear this has to come from someone else writing firmware for Plan B.


Things about the SD_read tab:

- While writing the SD_read tab, I encountered a mysterious problem where the counters refused to write properly to nozzle 1 and nozzle 2 (N0 and N1). Somehow the arrays that
  store the values for the nozzles didn't set the right values for the first 2 bits. This was solved by shifting the entire array 2 places. It now has 14 slots and goes from 2 to 13
  Ignoring 0 and 1. This does mean that there are some strange values in the SD_read block, but for now, this has solved the problem and I am not going to find a better fix anytime 
  soon.
- SD read should not be activated manually. The buffer is currently the only block that calls this function. I have tried calling for it manually when for instance depleting the 
  file (see printing tab), but by doing it manually, the file wouldn't open the next time. Just let everything SD related be handled by the buffer and request values though there.
  

