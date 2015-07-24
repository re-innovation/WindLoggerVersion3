#ifndef _BATTERY_H_
#define _BATTERY_H_

// Defines
#define battVoltagePin A1   // The battery voltage with a potential divider (470k//100k)

// Public Functions
void updateBatteryVoltage(void);

char * getBatteryVoltageStr(void);

#endif