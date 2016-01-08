/*
 * temperature.cpp
 *
 * Application temperature functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

/*
 * Application Includes
 */

#include "app.h"
#include "utility.h"

/* 
 * Defines and Typedefs
 */

enum {
  T_KELVIN=0,
  T_CELSIUS,
  T_FAHRENHEIT
};

#define thermistor A0  // This is the analog pin for the thermistor

/* Thermistor data for use in thermistor_to_temperature function */
struct thermistor
{
	float B;
	float T0;
	float R0;
};

#if READ_TEMPERATURE == 1

// Choose one thermsitor (comment out the others)
//static struct thermistor s_thermistor = {4300.0f,298.15f,10000.0f};			// Epicos K164 10K
static struct thermistor s_thermistor = {4126.0f,298.15f,10000.0f};					// GT 10K
//static struct thermistor s_thermistor = {4090.0f,298.15f,47000.0f};	// Vishay 10K

static float s_tempC = 0;  // This holds the converted value of temperature
static char s_tempCstr[6];  // A string buffer to hold the converted string

/*
 * Private Functions
 */

/* thermistor_to_temperature
 * Outputs: 
 * 	the actual temperature (float)
 * Inputs:
 * 	1.AnalogInputNumber - analog input to read from
 * 	2.OuputUnit - output in celsius, kelvin or fahrenheit
 * 	3. Your balance resistor resistance in ohms
 *	4. Set true if thermistor is a pullup
 */

static float thermistor_to_temperature(int AnalogInputNumber, int OutputUnit, float R_Balance, bool pullup)
{
  float R,T,data;

  // Changes as using thermistor to ground:
  data = float(analogRead(AnalogInputNumber));
  
  if (pullup)
  {
  	R = (1024.0f*R_Balance/data)-R_Balance;
  }
  else
  {
  	R = (data*R_Balance)/(1024.0f-data);
  }

  T=1.0f/(1.0f/s_thermistor.T0+(1.0f/s_thermistor.B)*log(R/s_thermistor.R0));

  switch(OutputUnit) {
    case T_CELSIUS :
      T-=273.15f;
    break;
    case T_FAHRENHEIT :
      T=9.0f*(T-273.15f)/5.0f+32.0f;
    break;
    default:
    break;
  };

  return T;
}

/*
 * Public Functions
 */

void TEMP_WriteTemperatureToBuffer(FixedLengthAccumulator * accum)
{
  if (!accum) { return; }
  s_tempC = thermistor_to_temperature(thermistor, T_CELSIUS, 10000.0f, true);
  dtostrf(s_tempC,2,2,s_tempCstr);  // Convert the temperature value (double) into a string

  accum->writeString(s_tempCstr);

  if(APP_InDebugMode())
  {
    Serial.print("Therm: ");
    Serial.println(s_tempCstr);  
  }
}

#else

void TEMP_WriteTemperatureToBuffer(FixedLengthAccumulator * accum)
{
	(void)accum;
}

#endif
