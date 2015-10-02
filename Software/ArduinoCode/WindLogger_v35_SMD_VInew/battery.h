#ifndef _BATTERY_H_
#define _BATTERY_H_

// Defines
#define BATT_VOLTAGE_PIN A1   // The battery voltage with a potential divider (470k//100k)

// Public Functions
void BATT_UpdateBatteryVoltage(void);

int BATT_WriteVoltageToBuffer(char * buffer);

#endif
