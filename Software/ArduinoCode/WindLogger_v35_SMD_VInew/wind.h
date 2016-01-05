#ifndef _WIND_H_
#define _WIND_H_

// Defines
#define VANE_PIN A2      // The wind vane with a 10k pull DOWN
#define ANEMOMETER1 3  //   This is digital pin the pulse is attached to
#define ANEMOMETER2 5  //   This is digital pin the pulse is attached to

// Public Functions

void WIND_SetupWindPulseInterrupts();

void WIND_ConvertWindDirection(int reading);
void WIND_AnalyseWindDirection();

void WIND_WritePulseCountToBuffer(uint8_t counter, FixedLengthAccumulator * accum);
void WIND_WriteDirectionToBuffer(FixedLengthAccumulator * accum);

long WIND_GetLivePulseCount(uint8_t counter);

void WIND_StoreWindPulseCounts();
void WIND_Debug();

#endif