/*
 * eeprom_storage.cpp
 *
 * Application EEPROM non-voltatile storage functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

/************ External Libraries*****************************/
#include <Arduino.h>
#include <EEPROM.h>

/*
 * Public Functions
 */

void EEPROM_GetDeviceID(char * buffer)
{
	if (buffer)
	{
		buffer[0] = (char)EEPROM.read(0);
		buffer[1] = (char)EEPROM.read(1);
	}
}

void EEPROM_SetDeviceID(char * buffer)
{
	if (buffer)
	{
		EEPROM.write(0,buffer[0]);
		EEPROM.write(1,buffer[1]);
	}
}

uint16_t EEPROM_GetSampleTime(void)
{
	return (EEPROM.read(2) << 8) + EEPROM.read(3);	
}

void EEPROM_SetSampleTime(uint16_t sampleTime)
{
	EEPROM.write(2, sampleTime >> 8);
    EEPROM.write(3, sampleTime & 0xff);
}

uint16_t EEPROM_GetCurrentOffset(void)
{
	return (EEPROM.read(4) << 8) + EEPROM.read(5);
}

void EEPROM_SetCurrentOffset(uint16_t currentOffset)
{
    EEPROM.write(4, currentOffset >> 8);
    EEPROM.write(5, currentOffset & 0xff);  
}

uint16_t EEPROM_GetR1(void)
{
	return (EEPROM.read(6) << 8) + EEPROM.read(7);	
}

void EEPROM_SetR1(uint16_t r1)
{
    EEPROM.write(6, r1 >> 8);
    EEPROM.write(7, r1 & 0xff);  
}

uint16_t EEPROM_GetR2(void)
{
	return (EEPROM.read(8) << 8) + EEPROM.read(9);	
}

void EEPROM_SetR2(uint16_t r2)
{
    EEPROM.write(8, r2 >> 8);
    EEPROM.write(9, r2 & 0xff);  
}

uint16_t EEPROM_GetCurrentGain(void)
{
	return (EEPROM.read(10) << 8) + EEPROM.read(11);	
}

void EEPROM_SetCurrentGain(uint16_t currentGain)
{
    EEPROM.write(10, currentGain >> 8);
    EEPROM.write(11, currentGain & 0xff);  
}
