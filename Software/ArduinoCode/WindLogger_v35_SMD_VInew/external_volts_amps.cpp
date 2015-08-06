/*
 * external_volts_amps.cpp
 *
 * External voltage and current functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "external_volts_amps.h"
#include "eeprom_storage.h"

/* 
 * Private Variables
 */

///********* External Voltage ****************/
static int  s_r1, s_r2;  // The potential divider values  
static float s_externalVoltage;        // Temporary store for float
static char  s_externalVoltStr[6];      // Hold the battery voltage as a string

///********* Current 1 ****************/
static long int currentData1;      // Temp holder for value
static float current1;        // Temporary store for float
static float currentOffset;  // Holds the offset current
static int currentOffsetInt;  // Holds the current offset as an in for EEPROM storing
static char s_current1Str[7];      // Hold the current as a string
static int s_iGain;    // Holds the current conversion factor in mV/A

/* 
 * Public Functions
 */

/* 
 * setCurrentOffset
 * Called by application to setup current offset
 */
void VA_SetCurrentOffset(int newOffset)
{
	currentOffsetInt = newOffset;
	// Convert the current offset to a voltage
  	currentOffset = float(currentOffsetInt)*3.3f/1023.0f;
}

/* 
 * setCurrentGain
 * Called by application to setup current gain
 */
void VA_SetCurrentGain(int newGain)
{
	s_iGain = newGain;
}

/* 
 * setVoltageDivider
 * Called by application to set the voltage divider parameters
 */
void VA_SetVoltageDivider(uint16_t newR1, uint16_t newR2)
{
	s_r1 = newR1;
	s_r2 = newR2;
}

/* 
 * VA_StoreNewCurrentOffset
 * Called by application to read a new offset at 0A
 * and store in EEPROM
 */
void VA_StoreNewCurrentOffset(void)
{
    currentData1 = 0;  // Reset this holder
    for(int i = 0;i<=19;i++)
    {  
      currentData1 += analogRead(CURRENT_1_PIN);
      delay(20);
    }           
    currentOffsetInt = currentData1/20;
    
    currentOffset = float(currentOffsetInt)*3.3f/1023.0f;
    Serial.print("Ioffset:");
    Serial.print(currentOffset);
    Serial.println("V");               // Write this info to EEPROM   
    EEPROM_SetCurrentOffset(currentOffsetInt);
}

/* 
 * VA_StoreNewResistor1
 * VA_StoreNewResistor2
 * Called by application to set new R1 and R2 values
 * and store in EEPROM
 */
void VA_StoreNewResistor1(int value)
{
    s_r1 = value;  // Use this new value
    Serial.print("R1:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetR1(value);    
}

void VA_StoreNewResistor2(int value)
{
    s_r2 = value; // Use this new value
    Serial.print("R2:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetR2(value);
}

/* 
 * VA_StoreNewCurrentGain
 * Called by application to set a new current gain
 * and store in EEPROM
 */
void VA_StoreNewCurrentGain(int value)
{
	s_iGain = value; // Use this new value
    Serial.print("I Gain:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetCurrentGain(value);
}

/* 
 * VA_UpdateExternalVoltage
 * Called by application to read the external voltage
 */
void VA_UpdateExternalVoltage(void)
{
	s_externalVoltage = float(analogRead(VOLTAGE_PIN))*(3.3f/1023.0f)*((float(s_r1)+float(s_r2))/float(s_r2));
    dtostrf(s_externalVoltage,2,2, s_externalVoltStr);
}

/* 
 * VA_UpdateExternalCurrent
 * Called by application to read the external current
 */
void VA_UpdateExternalCurrent(void)
{
	currentData1 = 0;  // Reset the value

    // Lets average the data here over 20 samples.
    for(int i = 0;i<=19;i++)
    {  
      currentData1 += analogRead(CURRENT_1_PIN);
      delay(2);
    }

    current1 = float(currentData1)/20.0f;  
    current1 = (current1*3.3f/1023.0f) - currentOffset;
    // Current 1 holds the incoming voltage.
     
    // ********** LEM HTFS 200-P SENSOR *********************************
    // Voutput is Vref +/- 1.25 * Ip/Ipn 
    // Vref = Vsupply/2 +/1 0.025V (Would be best to remove this with analog stage)
    //current1 = (current1*200.0f)/1.25f;
    current1 = current1*float(s_iGain);  
  
//    // ************* ACS*** Hall Effect **********************
//    // Output is Input Voltage - offset / mV per Amp sensitivity
//    // Datasheet says 60mV/A     

    // Convert the current to a string.
    dtostrf(current1,2,2, s_current1Str);     // Hold the battery voltage as a string
}

/* 
 * VA_GetExternalVoltageStr
 * Called by application to get the latest voltage string
 */
char * VA_GetExternalVoltageStr(void)
{
	return  s_externalVoltStr;
}

/* 
 * VA_GetExternalCurrentStr
 * Called by application to get the latest current string
 */
char * VA_GetExternalCurrentStr(void)
{
	return s_current1Str;
}
