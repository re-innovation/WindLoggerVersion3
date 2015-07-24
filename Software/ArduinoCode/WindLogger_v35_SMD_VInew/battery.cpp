/*
 * battery.cpp
 *
 * Application battery functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

#include "battery.h"

/* 
 * Private Variables
 */
static char s_batteryVoltStr[6];      // Hold the battery voltage as a string

/* 
 * Public Functions
 */

/* 
 * updateBatteryVoltage
 * Called by application to set new battery voltage from ADC reading
 */
void updateBatteryVoltage(uint16_t adc)
{
	// *********** BATTERY VOLTAGE ***************************************
    // From Vcc-470k-DATA-100k-GND potential divider
    // This is to test in case battery voltage has dropped too low - alert?
    float batteryVoltage;        // Temporary store for float
    batteryVoltage = float(adc)*(3.3f/1024.0f)*((470.0f+100.0f)/100.0f);        // Temporary store for float
    dtostrf(batteryVoltage, 2, 2, s_batteryVoltStr);     // Hold the battery voltage as a string
}

char * getBatteryVoltageStr(void)
{
	return s_batteryVoltStr;
}