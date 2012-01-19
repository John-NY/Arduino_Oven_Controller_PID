MAX31855 Arduino Library
========================

This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/">Creative Commons Attribution-ShareAlike 3.0 Unported License</a>.

<a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/3.0/88x31.png" /></a>
	
Developed By:

*   Alec N., Control Connection
*	Ryan McLaughlin <ryan@ryanjmclaughlin.com>


Summary
-------

The primary use of the library is to easily interface with a MAX31855 chip via the SPI interface.  Use the following code to initialize the library.

	MAX31855 temp(SCK,CS,SO);
	
Variables:	

*	CS is chip select pin of chip
*	SO is data out of the chip
*	SCK is the clock pin of the chip


Hardware
--------

There are many places you can find MAX31855 interface boards, or you can integrate it into your own design.  If you are looking for a breakout, you can check out [ryanjmclaughlin.com](http://ryanjmclaughlin.com). 


TODO
----

*	read_max31855.pde: Clean up code and comment!!  
*	read_max31855.pde: make use of all library functions and make more robust.
*	finish documenting functions in readme


Functions
---------

### readMAX31855
	bool readMAX31855(double *tempTC, double *tempCJC, bool *faultOpen, bool *faultShortGND, bool *faultShortVCC);
	
	x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
### readCelsius
	double readCelsius(void);
### readFarenheit
	double readFarenheit(void);
### readCJC
	double readCJC(void);
### readFaultCode
	uint8_t readFaultCode(void);
