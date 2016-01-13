/*
 * irradiance.cpp
 *
 * Application irradiance functionality for Wind Data logger
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


#if READ_IRRADIANCE == 1

/* 
 * Defines and Typedefs
 */

#define IRRADIANCE_PIN A2

/*
 * Local Variables
 */

const char s_pstr_irradiance_dbg[] PROGMEM = "Irradiance: ";

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

static float reading_to_irridiance(uint16_t reading)
{
  // From testing Approx 1mV = 1.1w/m2
  // This conversion is APPROXIMATE and from testing.
  float result = (reading * 3000.0f )/ 1024;
  return result; 
}

/*
 * Public Functions
 */

void IRR_WriteIrradianceToBuffer(FixedLengthAccumulator * accum)
{
  if (!accum) { return; }
  
  uint16_t reading = analogRead(IRRADIANCE_PIN);
  float irr = reading_to_irridiance(reading);
  
  char buffer[10];
  dtostrf(irr, 2, 0, buffer);  // Convert the irradiance value (double) into a string

  accum->writeString(buffer);

  if(APP_InDebugMode())
  {
    Serial.print(PStringToRAM(s_pstr_irradiance_dbg));
    Serial.println(buffer);  
  }
}

#else

void IRR_WriteIrradianceToBuffer(FixedLengthAccumulator * accum)
{
	(void)accum;
}

#endif
