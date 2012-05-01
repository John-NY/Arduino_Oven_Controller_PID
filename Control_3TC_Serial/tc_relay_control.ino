
#include <MAX31855.h>
#include <PID_v1.h>
#include "parameters.h"

// Set initial PID setpoints
double initialSetpoint = 20.0; // initial
double finalSetpoint = 20; // goal
double heating_rate = 1; // degree per minute
double maximum_permissible_gradient = 999; // degrees C, among TCs
unsigned long initialTime = 0;

// Define Thermocouple Variables we'll be connecting to
double Setpoint, Output;
double tempTC, tempCJC;
double tempTC2, tempCJC2;
double tempTC3, tempCJC3;
bool faultOpen, faultShortGND, faultShortVCC, x; // For TC#1
bool faultOpen2, faultShortGND2, faultShortVCC2; // For TC#2
bool faultOpen3, faultShortGND3, faultShortVCC3; // For TC#3

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

MAX31855 temp(13, 10, 12); // TC#1 SCK, measure, and fault pins
MAX31855 temp2(13,  9, 12); // TC#2 SCK, measure, and fault pins
MAX31855 temp3(13,  8, 12); // TC#3 SCK, measure, and fault pins

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

double GetGrad()
{
  return maximum_permissible_gradient;
}

void SetGrad(double fGrad)
{
  maximum_permissible_gradient = fGrad;
}

boolean Check_Gradient_Okay(double Setpoint, double TC1, double TC2) 
{ // Returns true if okay
    boolean delta1 =(TC1 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta2 =(TC2 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    return (delta1 && delta2);
}
boolean Check_Gradient_Okay(double Setpoint, double TC1, double TC2, double TC3) 
{ // Returns true if okay
    boolean delta1 =(TC1 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta2 =(TC2 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta3 =(TC3 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
/*
    Serial.print(maximum_permissible_gradient);
    Serial.print("\t");
    Serial.print((TC1 - Setpoint));
    Serial.print("\t");
    Serial.print((TC2 - Setpoint));
    Serial.print("\t");
    Serial.print((TC3 - Setpoint));
    Serial.print("\t");
    if (delta1 && delta2 && delta3) {
        Serial.print("true");
    } else {
        Serial.print("false");
    }
    Serial.println("");
*/
    return (delta1 && delta2 && delta3);
}
boolean Check_Overshoot_Okay(double Setpoint, double TC1, double TC2, double TC3, double TC4)
{ // Returns true if okay
    boolean delta1 =(TC1 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta2 =(TC2 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta3 =(TC3 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    boolean delta4 =(TC4 - Setpoint) < maximum_permissible_gradient; // ok if TC < target
    return (delta1 && delta2 && delta3 && delta4);
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
    x = temp2.readMAX31855(&tempTC2, &tempCJC2, &faultOpen, &faultShortGND, &faultShortVCC);
    x = temp3.readMAX31855(&tempTC3, &tempCJC3, &faultOpen, &faultShortGND, &faultShortVCC);
    double gap = abs(Setpoint-tempTC); //distance away from setpoint
     
    myPID.Compute();
  
    /************************************************
     * turn the output pin on/off based on pid output
     ************************************************/
    if(time_elapsed - windowStartTime>WindowSize)
    { //time to shift the Relay Window
        windowStartTime += WindowSize;
        //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
        reportResult3(Setpoint,tempTC,tempTC2,tempTC3,tempCJC,tempCJC2,tempCJC3,faultOpen,faultShortGND,faultShortVCC,faultOpen2,faultShortGND2,faultShortVCC2,faultOpen3,faultShortGND3,faultShortVCC3,Output,WindowSize);
    }
    
    if(Output > time_elapsed - windowStartTime)
    { // if we're in the "on" part of the duty cycle
        if(Output > minimumRelayTime)
        { // check to see if it's worth turning on.
            if (Check_Gradient_Okay(Setpoint,tempTC,tempTC2,tempTC3)) 
            { // keep gradient checker inside the loop so it runs infrequently
                digitalWrite(RELAY,HIGH);
            } else {
                digitalWrite(RELAY,LOW);
            }
        }
        //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
        reportResult3(Setpoint,tempTC,tempTC2,tempTC3,tempCJC,tempCJC2,tempCJC3,faultOpen,faultShortGND,faultShortVCC,faultOpen2,faultShortGND2,faultShortVCC2,faultOpen3,faultShortGND3,faultShortVCC3,Output,WindowSize);
        delay(minimumRelayTime);
        //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,WindowSize);
    } else digitalWrite(RELAY,LOW);
}


