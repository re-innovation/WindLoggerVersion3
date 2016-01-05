/*
* serial_handler.cpp
*
* Application serial handling functionality for Wind Data logger
*
* Matt Little/James Fowkes
* August 2015
*/

#include <Arduino.h>
#include <Rtc_Pcf8563.h>

/************ Application Libraries*****************************/

#include "serial_handler.h"
#include "eeprom_storage.h"
#include "sd.h"
#include "rtc.h"
#include "utility.h"
#include "external_volts_amps.h"

/*
 * Private Variables
 */

static char s_next_byte;
static char s_strBuffer[128];
static int s_index = 0;

const char reference[] PROGMEM = "The ref is:";

/*
 * Private Functions
 */

/*
 * setReferenceFromBuffer
 * Sets the two-byte device ID
 */
static void setReferenceFromBuffer(int i)
{
    SD_SetDeviceID(&s_strBuffer[i+1]);
    EEPROM_SetDeviceID(&s_strBuffer[i+1]);

    Serial.print(PStringToRAM(reference));
    Serial.print(s_strBuffer[i+1]);
    Serial.println(s_strBuffer[i+2]);
    SD_CreateFileForToday();
}

/*
 * setTimeFromBuffer
 * Sets the RTC time
 */
static void setTimeFromBuffer(int i)
{
    char temp[] = "00";
    temp[0] = s_strBuffer[i+1]; temp[1] = s_strBuffer[i+2];
    int hour = atoi(temp);

    temp[0] = s_strBuffer[i+3]; temp[1] = s_strBuffer[i+4];
    int minute = atoi(temp);

    temp[0] = s_strBuffer[i+5]; temp[6] = s_strBuffer[i+4];
    int second = atoi(temp);
    
    //hr, min, sec into Real Time Clock
    RTC_SetTime(hour, minute, second);

    SD_CreateFileForToday();

    Serial.println(RTC_GetTime());
}

/*
 * setDateFromBuffer
 * Sets the RTC date
 */
static void setDateFromBuffer(int i)
{
    char temp[] = "00";
    temp[0] = s_strBuffer[i+1]; temp[1] = s_strBuffer[i+2];
    int day = atoi(temp);

    temp[0] = s_strBuffer[i+3]; temp[1] = s_strBuffer[i+4];
    int month = atoi(temp);

    temp[0] = s_strBuffer[i+5]; temp[6] = s_strBuffer[i+4];
    int year = atoi(temp);
    
    RTC_SetDate(day, month, year);

    SD_CreateFileForToday();

    Serial.println(RTC_GetDate(RTCC_DATE_WORLD));
}

/*
 * setSampleTimeFromBuffer
 * Sets the sampling rate (in seconds)
 */
static void setSampleTimeFromBuffer(int i)
{
    long sampleTime = atol(&s_strBuffer[i+1]);  // Convert the string to a long int

    EEPROM_SetSampleTime((uint16_t)sampleTime);              

    Serial.print("Sample Time:");
    Serial.println(sampleTime);

    SD_ResetCounter();
}

/*
* Public Functions
*/

/*
 * SERIAL_HandleCalibrationData
 * Reads calibration settings from serial and hands off processing to appropriate functions
 */
void SERIAL_HandleCalibrationData()
{
    if (Serial.available() > 0) 
    {
        s_next_byte = Serial.read(); 
        s_strBuffer[s_index++] = s_next_byte;

        if (s_next_byte=='E')    // We read everything up to the byte 'E' which stands for END
        {
            int buffer_length = strlen(s_strBuffer);  // We also find the length of the string so we know how many char to display 
            // Depending upon what came before we update different values
            // To change the reference number we enter R00E, where 00 can be any number up to 99 

            for (int i = buffer_length; i>=0; i--)  // Check the buffer from the end of the data, working backwards
            {
                if (s_strBuffer[i]=='R')
                {
                    setReferenceFromBuffer(i);
                }

                if(s_strBuffer[i]=='T')
                {
                    setTimeFromBuffer(i);
                }

                if(s_strBuffer[i]=='D')
                {
                    setDateFromBuffer(i);
                }           

                if(s_strBuffer[i]=='S')
                {          
                    setSampleTimeFromBuffer(i);
                }

                if(s_strBuffer[i]=='O')
                {    
                    VA_StoreNewCurrentOffset();
                }

                if(s_strBuffer[i]=='V'&&s_strBuffer[i+1]=='1')
                {
                    char temp[] = "000";
                    temp[0] = s_strBuffer[i+2];
                    temp[1] = s_strBuffer[i+3];
                    temp[2] = s_strBuffer[i+4];
                    int value = atoi(temp);
                    VA_StoreNewResistor1(value);
                }

                if(s_strBuffer[i]=='V'&&s_strBuffer[i+1]=='2')
                {    
                    char temp[] = "000";
                    temp[0] = s_strBuffer[i+2];
                    temp[1] = s_strBuffer[i+3];
                    temp[2] = s_strBuffer[i+4];
                    int value = atoi(temp);
                    VA_StoreNewResistor2(value);
                }

                if(s_strBuffer[i]=='I')
                {    
                    char temp[] = "000";
                    temp[0] = s_strBuffer[i+1];
                    temp[1] = s_strBuffer[i+2];
                    temp[2] = s_strBuffer[i+3];
                    int value = atoi(temp);
                    VA_StoreNewCurrentGain(value);
                }          
            }
            s_strBuffer[0] = '\0';
            s_index = 0;  // Reset the buffer to be filled again 
        }
    }
}
