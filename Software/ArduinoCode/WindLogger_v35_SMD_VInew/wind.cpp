/*
 * wind.cpp
 *
 * Application wind direction/speed functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

#include "wind.h"

/* 
 * Private Variables
 */
/********** Wind Direction Storage *************/
static String WindDirection = " ";  // Empty to start with
static int windDirectionArray[] = {0,0,0,0,0,0,0,0};  //Holds the frequency of the wind direction

// Variables for the Pulse Counter
static volatile long pulseCounter1 = 0;  // This counts pulses from the flow sensor  - Needs to be long to hold number
static volatile long pulseCounter1Old = 0;  // This is storage for the old flow sensor - Needs to be long to hold number

static volatile long pulseCounter2 = 0;  // This counts pulses from the flow sensor  - Needs to be long to hold number
static volatile long pulseCounter2Old = 0;  // This is storage for the old flow sensor - Needs to be long to hold number

/* 
 * Private Functions
 */

/***************************************************
 *  Name:        pulse1
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Count pulses from Anemometer 1
 *
 ***************************************************/
static void pulse1(void)
{
  // If the anemometer has spun around
  // Increment the pulse counter
  pulseCounter1++;
  // ***TO DO**** Might need to debounce this
}

/***************************************************
 *  Name:        pulse2
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Count pulses from Anemometer 2
 *
 ***************************************************/
static void pulse2(void)
{
  // If the anemometer has spun around
  // Increment the pulse counter
  pulseCounter2++;
  // ***TO DO**** Might need to debounce this
}

/* 
 * Public Functions
 */

/* 
 * setupWindPulseInterrupts
 * Configures pins and interrupts for pulse counting
 */
void setupWindPulseInterrupts()
{
	pinMode(ANEMOMETER1, INPUT); 
	digitalWrite(ANEMOMETER1, HIGH);
	enableInterrupt(ANEMOMETER1, &pulse1, FALLING);
	pinMode(ANEMOMETER2, INPUT); 
	digitalWrite(ANEMOMETER2, HIGH);
	enableInterrupt(ANEMOMETER2, &pulse2, FALLING); 
}

// ******** CALC DIRECTION *********
// This routine takes in an analog read value and converts it into a wind direction
// The Wind vane uses a series of resistors to show what direction the wind comes from
// The different values are (with a 10k to Ground):
//    R1 = 33k  => 238 N
//    R2 = 8.2k => 562 NE
//    R3 = 1k => 930 E
//    R4 = 2.2k => 839 SE
//    R5 = 3.9k => 736 S
//    R6 = 16k => 394 SW
//    R7 = 120k => 79 W
//    R8 = 64.9k => 137 NW

// The different values are (with a 10k to Vbattery):
// The value will be 1024 - vane integer reading

// This means we can 'band' the data into 8 bands

void convertWindDirection(int reading)
{

 // The reading has come from the ADC
	if(reading>0&&reading<100)
	{
		windDirectionArray[6]++;
	}
	else if(reading>100&&reading<200)
	{
		windDirectionArray[7]++;
	}
	else if(reading>200&&reading<350)
	{
		windDirectionArray[0]++; 
	}
	else if(reading>350&&reading<450)
	{
		windDirectionArray[5]++;
	}  
	else if(reading>450&&reading<650)
	{
		windDirectionArray[1]++;
	}  
	else if(reading>650&&reading<800)
	{
		windDirectionArray[4]++;
	}
	else if(reading>800&&reading<900)
	{
		windDirectionArray[3]++;
	}
	else if(reading>900&&reading<1024)
	{
		windDirectionArray[2]++;
	}
	else
	{
	  // This is an error reading
	}
}

void analyseWindDirection()
{
 // When a data sample period is over we need to see the most frequent wind direction.
 // This needs to be converted back to a direction and stored on SD

	int data1 = windDirectionArray[0];
	int maxIndex = 0;
 // First need to find the maximum integer in the array
	for(int i=1;i<8;i++)
	{
		if(data1<windDirectionArray[i])
		{
			data1=windDirectionArray[i];
			maxIndex = i;
		}
	}
 // Serial.println(maxIndex);  Testing


 // Then convert that into the direction
	switch(maxIndex)
	{
		case 0:
			WindDirection = "N";
			break;
		case 1:
			WindDirection = "NE";
			break;    
		case 2:
			WindDirection = "E";
			break;  
		case 3:
			WindDirection = "SE";
			break;
		case 4:
			WindDirection = "S";
			break;  
		case 5:
			WindDirection = "SW";
			break;
		case 6:
			WindDirection = "W";
			break;
		case 7:
			WindDirection = "NW";
			break;
	}

	for(int i=0;i<8;i++)
	{
		//Resets the wind direction array
		windDirectionArray[i]=0;
	}
}

/* 
 * getWindDirectionStr
 * Called by application to get the latest wind direction string
 */
String& getWindDirectionStr()
{
	return WindDirection;
}

/* 
 * getPulseCountStr
 * Called by application to get the latest pulse count string
 */
String& getPulseCountStr(uint8_t counter)
{
	static String pulseCountString;

	switch(counter)
	{
		case 0:
			pulseCountString = String(pulseCounter1Old);
			break;
		case 1:
			pulseCountString = String(pulseCounter1Old);
			break;
		default:
			pulseCountString = String("??");
			break;
	}

	return pulseCountString;
}

/* 
 * getLivePulseCount
 * Called by application to get the live pulse count
 */
long getLivePulseCount(uint8_t counter)
{
	long count;

	switch(counter)
	{
		case 0:
			count = pulseCounter1;
			break;
		case 1:
			count = pulseCounter2;
			break;
		default:
			count = 0;
			break;
	}

	return count;
}

/* 
 * getWindDirectionStr
 * Saves the latest pulse counts and resets the live counts
 */
void storeWindPulseCounts()
{
	pulseCounter1Old = pulseCounter1;
    pulseCounter2Old = pulseCounter2;
    // Reset the pulse counters
    pulseCounter1 = 0;
    pulseCounter2 = 0;
}
