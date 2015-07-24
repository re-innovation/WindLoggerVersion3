/*
 * external-volts-amps.cpp
 *
 * External voltage and current functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "external-volts-amps.h"

/* 
 * Private Variables
 */

///********* External Voltage ****************/
static int  r1,r2;  // The potential divider values  
static float externalVoltage;        // Temporary store for float
static char ExternalVoltStr[6];      // Hold the battery voltage as a string

///********* Current 1 ****************/
static long int currentData1;      // Temp holder for value
static float current1;        // Temporary store for float
static float currentOffset;  // Holds the offset current
static int currentOffsetInt;  // Holds the current offset as an in for EEPROM storing
static char Current1Str[7];      // Hold the current as a string
static int iGain;    // Holds the current conversion factor in mV/A

/* 
 * Public Functions
 */

/* 
 * setCurrentOffset
 * Called by application to setup current offset
 */
void setCurrentOffset(uint8_t hi, uint8_t lo)
{
	currentOffsetInt = (hi << 8)+lo;
	// Convert the current offset to a voltage
  	currentOffset = float(currentOffsetInt)*3.3f/1023.0f;
}

/* 
 * setCurrentGain
 * Called by application to setup current gain
 */
void setCurrentGain(uint8_t hi, uint8_t lo)
{
	iGain = (hi << 8)+lo;
}

/* 
 * setVoltageDivider
 * Called by application to set the voltage divider parameters
 */
void setVoltageDivider(uint8_t hiR1, uint8_t loR1, uint8_t hiR2, uint8_t loR2)
{
	r1 = (hiR1 << 8)+loR1;
	r2 = (hiR2 << 8)+loR2;
}

/* 
 * storeNewCurrentOffset
 * Called by application to read a new offset at 0A
 * and store in EEPROM
 */
void storeNewCurrentOffset(void)
{
    currentData1 = 0;  // Reset this holder
    for(int i = 0;i<=19;i++)
    {  
      currentData1 += analogRead(current1Pin);
      delay(20);
    }           
    currentOffsetInt = currentData1/20;
    
    currentOffset = float(currentOffsetInt)*3.3f/1023.0f;
    Serial.print("Ioffset:");
    Serial.print(currentOffset);
    Serial.println("V");               // Write this info to EEPROM   
    EEPROM.write(4, currentOffsetInt >> 8);    // Do this seperately
    EEPROM.write(5, currentOffsetInt & 0xff);  
}

/* 
 * storeNewResistor1
 * storeNewResistor2
 * Called by application to set new R1 and R2 values
 * and store in EEPROM
 */
void storeNewResistor1(int value)
{
    r1 = value;  // Use this new value
    Serial.print("R1:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM.write(6, value >> 8);    // Do this seperately
    EEPROM.write(7, value & 0xff);  
}

void storeNewResistor2(int value)
{
    r2 = value; // Use this new value
    Serial.print("R2:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM.write(8, value >> 8);    // Do this seperately
    EEPROM.write(9, value & 0xff);  
}

/* 
 * storeNewCurrentGain
 * Called by application to set a new current gain
 * and store in EEPROM
 */
void storeNewCurrentGain(int value)
{
	iGain = value; // Use this new value
    Serial.print("I Gain:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM.write(10, value >> 8);    // Do this seperately
    EEPROM.write(11, value & 0xff);  
}

void updateExternalVoltage(void)
{
	externalVoltage = float(analogRead(voltagePin))*(3.3f/1023.0f)*((float(r1)+float(r2))/float(r2));
    dtostrf(externalVoltage,2,2,ExternalVoltStr);
}

void updateExternalCurrent(void)
{
	currentData1 = 0;  // Reset the value

    // Lets average the data here over 20 samples.
    for(int i = 0;i<=19;i++)
    {  
      currentData1 += analogRead(current1Pin);
      delay(2);
    }

    current1 = float(currentData1)/20.0f;  
    current1 = (current1*3.3f/1023.0f) - currentOffset;
    // Current 1 holds the incoming voltage.
     
    // ********** LEM HTFS 200-P SENSOR *********************************
    // Voutput is Vref +/- 1.25 * Ip/Ipn 
    // Vref = Vsupply/2 +/1 0.025V (Would be best to remove this with analog stage)
    //current1 = (current1*200.0f)/1.25f;
    current1 = current1*float(iGain);  
  
//    // ************* ACS*** Hall Effect **********************
//    // Output is Input Voltage - offset / mV per Amp sensitivity
//    // Datasheet says 60mV/A     

    // Convert the current to a string.
    dtostrf(current1,2,2,Current1Str);     // Hold the battery voltage as a string
}

char * getExternalVoltageStr(void)
{
	return ExternalVoltStr;
}

char * getExternalCurrentStr(void)
{
	return Current1Str;
}
