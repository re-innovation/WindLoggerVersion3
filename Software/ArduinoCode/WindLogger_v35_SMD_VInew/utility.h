#ifndef _UTILITY_H_
#define _UTILITY_H_

// Converts a decimal to BCD (binary coded decimal)
byte DecToBcd(byte value);
char* PStringToRAM(const PROGMEM char* str);

#endif