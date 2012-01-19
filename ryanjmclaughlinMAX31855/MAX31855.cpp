/*
  MAX31855.cpp - Library for reading temperature from a MAX31855.
  
  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
  http://creativecommons.org/licenses/by-sa/3.0/
*/

#include <avr/pgmspace.h>
#include <WProgram.h>
#include <stdlib.h>

#include "MAX31855.h"

MAX31855::MAX31855(int8_t SCLK, int8_t CS, int8_t MISO) {
  sclk = SCLK;
  cs = CS;
  miso = MISO;

  //define pin modes
  pinMode(cs, OUTPUT);
  pinMode(sclk, OUTPUT); 
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);
}

double MAX31855::readCelsius(void) {

  int16_t v;

  v = spiread16();
   //if fault bit set // return 2000deg
  if (v & 0x1) 
	  return 2000;    
  v&=0xfffc; // mask lower two bits
  return v / 16.0;
}

double MAX31855::readFarenheit(void) {
 	return readCelsius() * 9.0/5.0 + 32;
}
double MAX31855::readCJC(void) {

  int16_t v;
  v = spiread32() & 0xfff0;
  return v / 256.0;
}

uint8_t MAX31855::readFaultCode(void) {

return (spiread32() & 0x7) ; // return low 3 bits   
  	
}
uint32_t MAX31855::spiread32(void) { 
  int i;
  uint16_t d = 0; // we only need last 16 bits, first 16 will be discarded
   digitalWrite(cs, LOW);
  for (i=31; i>=0; i--)
	  {
		digitalWrite(sclk, LOW);
		if (digitalRead(miso))   
			d |= (1 << i);
		digitalWrite(sclk, HIGH);
	  }
	digitalWrite(cs, HIGH);

  return d; 
}
uint16_t MAX31855::spiread16(void) { 
  int i;
  uint16_t d = 0;
  digitalWrite(cs, LOW);
  for (i=15; i>=0; i--)
  {
	digitalWrite(sclk, LOW);
	if (digitalRead(miso))
		d |= (1 << i);
	digitalWrite(sclk, HIGH);
  }
  digitalWrite(cs, HIGH);

  return d; 
}

bool MAX31855::readMAX31855(double *tempTC, double *tempCJC, bool *faultOpen, bool *faultShortGND, bool *faultShortVCC){
	int i;
	int16_t d = 0;
	int16_t v = 0;
	bool fault = false;

	digitalWrite(cs, LOW);
	for (i=15; i>=0; i--)
		{
		digitalWrite(sclk, LOW);
		if (digitalRead(miso))
			d |= (1 << i);
		digitalWrite(sclk, HIGH);
		}
	for (i=15; i>=0; i--)
		{
		digitalWrite(sclk, LOW);
		if (digitalRead(miso))
			v |= (1 << i);
		digitalWrite(sclk, HIGH);
		}
	digitalWrite(cs, HIGH);
	
	if (d & 0x1)
	{
		fault=true;
		*tempTC =9999;
	} 
	else
	{
		 d&=0xfffc; // mask lower two bits
		 *tempTC = d / 16.0;
	}
	
	*faultOpen = (v & 0x1) ? true : false  ;
	*faultShortGND = ((v>>1) & 0x1) ? true : false;
	*faultShortVCC = ((v>>2) & 0x1) ? true : false;

	v = v & 0xfff0;// mask lower 4 bits
    *tempCJC = v / 256.0;

	return fault;
}
