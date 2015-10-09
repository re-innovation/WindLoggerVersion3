#ifndef _APP_H_
#define _APP_H_

/*
 * Defines and typedefs
 */

// If READ_TEMPERATURE is 1, the temperature will be read and included in serial data
#define READ_TEMPERATURE 1

/*
 * Application functions
 */

void APP_SecondTick();
bool APP_InDebugMode();

#endif
