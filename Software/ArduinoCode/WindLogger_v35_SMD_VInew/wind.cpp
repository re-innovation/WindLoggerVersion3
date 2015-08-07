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
static String s_windDirection = " ";  // Empty to start with
static int s_windDirectionArray[] = {0,0,0,0,0,0,0,0};  //Holds the frequency of the wind direction

// Variables for the Pulse Counter
static volatile long s_pulseCounters[2] = {0, 0};  // This counts pulses from the flow sensor  - Needs to be long to hold number
static volatile long s_pulseCountersOld[2] = {0, 0};  // This is storage for the old flow sensor - Needs to be long to hold number

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
  s_pulseCounters[0]++;
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
  s_pulseCounters[1]++;
  // ***TO DO**** Might need to debounce this
}

/* 
 * Public Functions
 */

/* 
 * WIND_SetupWindPulseInterrupts
 * Configures pins and interrupts for pulse counting
 */
void WIND_SetupWindPulseInterrupts()
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

void WIND_ConvertWindDirection(int reading)
{

 // The reading has come from the ADC
	if(reading>0&&reading<100)
	{
		s_windDirectionArray[6]++;
	}
	else if(reading>100&&reading<200)
	{
		s_windDirectionArray[7]++;
	}
	else if(reading>200&&reading<350)
	{
		s_windDirectionArray[0]++; 
	}
	else if(reading>350&&reading<450)
	{
		s_windDirectionArray[5]++;
	}  
	else if(reading>450&&reading<650)
	{
		s_windDirectionArray[1]++;
	}  
	else if(reading>650&&reading<800)
	{
		s_windDirectionArray[4]++;
	}
	else if(reading>800&&reading<900)
	{
		s_windDirectionArray[3]++;
	}
	else if(reading>900&&reading<1024)
	{
		s_windDirectionArray[2]++;
	}
	else
	{
	  // This is an error reading
	}
}

void WIND_AnalyseWindDirection()
{
 // When a data sample period is over we need to see the most frequent wind direction.
 // This needs to be converted back to a direction and stored on SD

	int data1 = s_windDirectionArray[0];
	int maxIndex = 0;
 // First need to find the maximum integer in the array
	for(int i=1;i<8;i++)
	{
		if(data1<s_windDirectionArray[i])
		{
			data1=s_windDirectionArray[i];
			maxIndex = i;
		}
	}
 // Serial.println(maxIndex);  Testing


 // Then convert that into the direction
	switch(maxIndex)
	{
		case 0:
			s_windDirection = "N";
			break;
		case 1:
			s_windDirection = "NE";
			break;    
		case 2:
			s_windDirection = "E";
			break;  
		case 3:
			s_windDirection = "SE";
			break;
		case 4:
			s_windDirection = "S";
			break;  
		case 5:
			s_windDirection = "SW";
			break;
		case 6:
			s_windDirection = "W";
			break;
		case 7:
			s_windDirection = "NW";
			break;
	}

	for(int i=0;i<8;i++)
	{
		//Resets the wind direction array
		s_windDirectionArray[i]=0;
	}
}

/* 
 * WIND_GetWindDirectionStr
 * Called by application to get the latest wind direction string
 */
String& WIND_GetWindDirectionStr()
{
	return s_windDirection;
}

/* 
 * WIND_GetPulseCountStr
 * Called by application to get the latest pulse count string
 */
String& WIND_GetPulseCountStr(uint8_t counter)
{
	static String pulseCountString;

	switch(counter)
	{
		case 0:
			pulseCountString = String(s_pulseCountersOld[0]);
			break;
		case 1:
			pulseCountString = String(s_pulseCountersOld[1]);
			break;
		default:
			pulseCountString = String("??");
			break;
	}

	return pulseCountString;
}

/* 
 * WIND_GetLivePulseCount
 * Called by application to get the live pulse count
 */
long WIND_GetLivePulseCount(uint8_t counter)
{
	long count;

	switch(counter)
	{
		case 0:
			count = s_pulseCounters[0];
			break;
		case 1:
			count = s_pulseCounters[1];
			break;
		default:
			count = 0;
			break;
	}

	return count;
}

/* 
 * WIND_GetWindDirectionStr
 * Saves the latest pulse counts and resets the live counts
 */
void WIND_StoreWindPulseCounts()
{
	s_pulseCountersOld[0] = s_pulseCounters[0];
    s_pulseCountersOld[1] = s_pulseCounters[1];
    // Reset the pulse counters
    s_pulseCounters[0] = 0;
    s_pulseCounters[1] = 0;
}
