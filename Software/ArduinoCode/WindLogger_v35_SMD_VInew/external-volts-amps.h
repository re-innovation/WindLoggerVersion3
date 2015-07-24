#ifndef _EXTERNAL_VOLTS_AMPS_H_
#define _EXTERNAL_VOLTS_AMPS_H_

// Defines
#define voltagePin A2  // The external voltage with a potential divider (680k // 46k)
#define current1Pin A3  // Current from a hall effect sensor

// Public Functions
void setCurrentOffset(uint8_t hi, uint8_t lo);
void setCurrentGain(uint8_t hi, uint8_t lo);

void setVoltageDivider(uint8_t hiR1, uint8_t loR1, uint8_t hiR2, uint8_t loR2);

void storeNewCurrentOffset(void);
void storeNewResistor1(int value);
void storeNewResistor2(int value);
void storeNewCurrentGain(int value);

void updateExternalVoltage(void);
void updateExternalCurrent(void);

char * getExternalVoltageStr(void);
char * getExternalCurrentStr(void);

#endif