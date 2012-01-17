
#include <MAX31855.h>
#include <PID_v1.h>
#include "parameters.h"

double initialSetpoint = 20.0; // initial
double finalSetpoint = 60; // goal
double heating_rate = 1; // degree per minute
unsigned long initialTime = 0;

//Define Variables we'll be connecting to
double Setpoint, Output;
double tempTC, tempCJC;
bool faultOpen, faultShortGND, faultShortVCC, x;

//Specify the links and initial tuning parameters
//Define the aggressive and conservative Tuning Parameters
double aggKp=400, aggKi=0.1, aggKd=50;
// Each degree centigrade as one point towards the decision
// to turn on or off.  Kp scales this up toward 5000.
double consKp=1, consKi=0.05, consKd=0;
PID myPID(&tempTC, &Output, &Setpoint, aggKp, aggKi, aggKd, DIRECT);

// Time window of 5000 milliseconds?
int WindowSize = 5000;
unsigned long windowStartTime; // each new increment

MAX31855 temp(13, 10, 12); // measure and fault pins

void TC_Relay_Init()
{
  
  windowStartTime = millis();
  pinMode(RELAY,OUTPUT);
  
  //initialize the variables we're linked to
  Setpoint = initialSetpoint;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  
}


double GetSetpoint()
{
  return finalSetpoint;
}

void SetSetpoint(double fSetPoint)
{
  initialTime = millis();
  windowStartTime = 0;
  initialSetpoint = tempTC;
  finalSetpoint = fSetPoint;
}

double GetRate()
{
  initialTime = millis();
  windowStartTime = 0;
  initialSetpoint = tempTC;
  return heating_rate;
}

void SetRate(double fRate)
{
  heating_rate = fRate;
}

void TC_Relay_Loop()
{
    unsigned long time_elapsed = millis() - initialTime;
  
    Setpoint = initialSetpoint + double(time_elapsed) * heating_rate / oneminute;
    if (Setpoint > finalSetpoint) 
    {
        Setpoint = finalSetpoint;
    }
     
    x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
    double gap = abs(Setpoint-tempTC); //distance away from setpoint
     
    myPID.Compute();
  
    /************************************************
     * turn the output pin on/off based on pid output
     ************************************************/
    if(time_elapsed - windowStartTime>WindowSize)
    { //time to shift the Relay Window
        windowStartTime += WindowSize;
        //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
        reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
    }
    
    if(Output > time_elapsed - windowStartTime)
    { // if we're in the "on" part of the duty cycle
        if(Output > minimumRelayTime)
        { // check to see if it's worth turning on.
            digitalWrite(RELAY,HIGH);
        }
        reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
        delay(minimumRelayTime);
        //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
    } else digitalWrite(RELAY,LOW);
}

