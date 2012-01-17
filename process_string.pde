/********************************************************
 * process_string
 *
 * Just like in RepRap, I want functions to parse and use
 * the serial commands.  Serial commands are delivered byte-
 * by-byte, so I'd like to steal my well-tested "arduino-gcode"
 * CNC control parser.  CNC Controller has been working well
 * for almost 2 years now.
 *******************************************************/

#include "parameters.h"

//our command string
char cmdbuffer[COMMAND_SIZE];
char c = '?';
byte serial_count = 0;
boolean comment = false;

int scan_int(char *str, int *valp);
int scan_float(char *str, float *valp);

/* gcode line parse results */
struct GcodeParser
{
    unsigned int seen;
    int M;
    float S;
    float R;
};

//init our string processing
void init_process_string()
{
	serial_count = 0;
        comment = false;
}
 
// Get a command and process it
void get_and_do_command()
{
	//read in characters if we got them.
	if (Serial.available())
	{
		c = Serial.read();
                if(c == '\r')
                  c = '\n';
                // Throw away control chars except \n
                if(c >= ' ' || c == '\n')
                {

		  //newlines are ends of commands.
		  if (c != '\n')
		  {
			// Start of comment - ignore any bytes received from now on
			if (c == ';')
				comment = true;
				
			// If we're not in comment mode, add it to our array.
			if (!comment)
				cmdbuffer[serial_count++] = c;
		  }

                }
	}

        // Data runaway?
        if(serial_count >= COMMAND_SIZE)
          init_process_string();

	//if we've got a real command, do it
	if (serial_count && c == '\n')
	{
                Serial.println(cmdbuffer);
                // Terminate string
                cmdbuffer[serial_count] = 0;
                
		//process our command!
		process_string(cmdbuffer, serial_count);

		//clear command.
		init_process_string();

                // Say we're ready for the next one
                
                Serial.println("ok");
	}
}

#define PARSE_INT(ch, str, len, val) \
	case ch: \
		len = scan_int(str, &val); \
		break;

#define PARSE_FLOAT(ch, str, len, val) \
	case ch: \
		len = scan_float(str, &val); \
		break;

int scan_float(char *str, float *valp)
{
	float res;
	int len;
	char *end;
     
	res = (float)strtod(str, &end);
      
	len = end - str;

	if (len > 0)
	{
		*valp = res;
	}
	else
		*valp = 0;
          
	return len;	/* length of number */
}

int scan_int(char *str, int *valp)
{
	int res;
	int len;
	char *end;

	res = (int)strtol(str, &end, 10);
	len = end - str;

	if (len > 0)
	{
		*valp = res;
	}
	else
		*valp = 0;
          
	return len;	/* length of number */
}

void process_string(char instruction[], int size)
{

	GcodeParser *gc;	/* string parse result */

	int ind;
	int len;	/* length of parameter argument */

	gc->seen = 0;
        gc->M = 0;
        
	len=0;
	/* scan the string for commands and parameters, recording the arguments for each,
	 * and setting the seen flag for each that is seen
	 */

	for (ind=0; ind<size; ind += (1+len))
	{
		len = 0;
		switch (instruction[ind])
		{
			  PARSE_INT('M', &instruction[ind+1], len, gc->M);
			PARSE_FLOAT('S', &instruction[ind+1], len, gc->S);
			PARSE_FLOAT('R', &instruction[ind+1], len, gc->R);
                        default:
			  break;
		}
	}

    Serial.print("M = ");
    Serial.print(gc->M);
    Serial.print(", S = ");
    Serial.print(gc->S);
    Serial.print(", R = ");
    Serial.print(gc->R);
    Serial.println(", ");

    double newSetpoint = GetSetpoint();
    double newRate = GetRate();
    if(gc->S >= TMIN)
    {
        if(gc->S <= TMAX)
        {
            newSetpoint = double(gc->S);
            SetSetpoint(newSetpoint);
        }
    }
    if(gc->R > RMIN)
    {
        if(gc->R < RMAX)
        {
            newRate = double(gc->R);
            SetRate(newRate);
        }
    }
    

    return;
}

void reportResult(double Setpoint, double tempTC, double tempCJC, 
                  bool faultOpen, bool faultShortGND, bool faultShortVCC,
                  double Output, int WindowSize)
{
      // Report during each new window (about 5000 milliseconds)
      Serial.print(Setpoint);
      Serial.print("\t");
      Serial.print(tempTC);
      Serial.print("\t");
      Serial.print(tempCJC);
      Serial.print("\t");
      Serial.print(faultOpen);
      Serial.print(faultShortGND);
      Serial.print(faultShortVCC); 
      Serial.print("\t"); 
      Serial.print(int(Output));
      Serial.print("\t");
      Serial.println(WindowSize);
}
