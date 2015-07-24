#ifndef _WIND_H_
#define _WIND_H_

// Defines
#define vanePin A0      // The wind vane with a 10k pull DOWN
#define pulseInterrupt 1  // Pulse Counter Interrupt - This is pin 3 of arduino - which is INT1
#define ANEMOMETER1 3  //   This is digital pin the pulse is attached to
#define ANEMOMETER2 5  //   This is digital pin the pulse is attached to

// Public Functions

void setupWindPulseInterrupts();

void convertWindDirection(int reading);
void analyseWindDirection();

String& getWindDirectionStr();
String& getPulseCountStr(uint8_t counter);
long getLivePulseCount(uint8_t counter);

void storeWindPulseCounts();

#endif