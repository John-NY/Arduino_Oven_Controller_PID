/*
  Control_TC_Serial
  
  Sets temperature ramp based on serial inputs.  At the very
  least, it should take 2 arguments (setpoint and slope), and
  change the temperature.

  // UPDATE 17 MARCH 2012 -- Enable Gradient Checking to 
  // reduce temperature overshoot.  In the Thermolyne 
  // furnace, I can build a 200C top-to-bottom gradient
  // (top is hotter).  By the time the bottom catches up
  // to the top, the heat reservoir at the top is enormous,
  // and dissipates into the bottom, raising the temperature
  // to 100 degrees above the setpoint.

  Based on PID Relay example and TC Shield examples.
  PID Relay example is weakly commented.  Analog example
  is much better.  Minimum time is ok?
  
  // I had to disable error checking to insert a negative Kd  
  // (a negative derivative coefficient).  I am sure I want a 
  // negative derivative coefficient because the system is mostly
  // off (not working hard).  If it stops working, the temperature
  // drops very slowly.  Thus it's never important to have a 
  // positive derivative coefficient.  
  // Would I instead use an integral coefficient to avoid changing
  // the PID source??   Then most of the "turn on" behavior is due 
  // to the cumulative lack of sufficient temperature.
  // the only problem is that I don't know how this parameter 
  // behaves over time -- how long before integral is flushed?
  // I guess it's the "Window Size" parameter -- 5000 millis.
  
  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
  http://creativecommons.org/licenses/by-sa/3.0/
*/

#include "parameters.h"

void setup() {
  Serial.begin(9600);
  TC_Relay_Init();
}

void loop() {
  get_and_do_command();
  TC_Relay_Loop();
}


