/*
  MAX31855.h - Library for reading temperature from a MAX31855.
  
  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
  http://creativecommons.org/licenses/by-sa/3.0/
*/

#include <WProgram.h>

class MAX31855 {
 public:
  MAX31855(int8_t SCLK, int8_t CS, int8_t MISO);
  bool readMAX31855(double *tempTC, double *tempCJC, bool *faultOpen, bool *faultShortGND, bool *faultShortVCC);
  double readCelsius(void);
  double readFarenheit(void);
  double readCJC(void);
  uint8_t readFaultCode(void);
 private:
  int8_t sclk, miso, cs;
  uint32_t spiread32(void);
  uint16_t spiread16(void);
};
