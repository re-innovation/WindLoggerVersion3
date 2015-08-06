/*
 * utility.cpp
 *
 * Application utility functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

/************ External Libraries*****************************/
#include <Arduino.h>

/************ Application Libraries*****************************/
#include "utility.h"

/* 
 * Defines
 */

#define MAX_STRING 130      // Sets the maximum length of string probably could be lower

/* 
 * Private Variables
 */

static char s_progmemBuffer[MAX_STRING];  // A buffer to hold the string when pulled from program memory


/* 
 * Public Functions
 */

/***************************************************
 *  Name:        DecToBcd
 *
 *  Returns:     BCD value
 *
 *  Parameters:  Decimal value
 *
 *  Description: Turns decimal value into BCD encoded value (e.g 12 => 0x12)
 *
 ***************************************************/
byte DecToBcd(byte value)
{
  return (value / 10 * 16 + value % 10);
}

/***************************************************
 *  Name:        PStringToRAM
 *
 *  Returns:     Pointer to local string buffer
 *
 *  Parameters:  Pointer to string in PROGMEM.
 *
 *  Description: Copies string from flash to RAM and returns pointer to the RAM copy
 *
 ***************************************************/
char* PStringToRAM(const char* str) {
	strcpy_P(s_progmemBuffer, (char*)str);
	return s_progmemBuffer;
}
