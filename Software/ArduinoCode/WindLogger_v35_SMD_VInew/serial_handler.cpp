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

static char next_byte;
static String str_buffer = "";  // This is the holder for the string which we will display

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
    SD_SetDeviceID(&str_buffer[i+1]);
    EEPROM_SetDeviceID(&str_buffer[i+1]);

    Serial.print(PStringToRAM(reference));
    Serial.print(str_buffer[i+1]);
    Serial.println(str_buffer[i+2]);
    SD_CreateFileForToday();
}

/*
 * setTimeFromBuffer
 * Sets the RTC time
 */
static void setTimeFromBuffer(int i)
{
    String hourstr = str_buffer.substring(i+1,i+3);
    int hour = atoi(&hourstr[0]);
    String minutestr = str_buffer.substring(i+3,i+5);
    int minute = atoi(&minutestr[0]);
    String secondstr = str_buffer.substring(i+5,i+7);
    int second = atoi(&secondstr[0]);
    
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
    String daystr = str_buffer.substring(i+1,i+3);
    int day = atoi(&daystr[0]);
    String monthstr = str_buffer.substring(i+3,i+5);
    int month = atoi(&monthstr[0]);          
    String yearstr = str_buffer.substring(i+5,i+7);
    int year = atoi(&yearstr[0]);          

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
    long sampleTime = atol(&str_buffer[i+1]);  // Convert the string to a long int

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
        next_byte = Serial.read(); 
        str_buffer += next_byte;

        if (next_byte=='E')    // We read everything up to the byte 'E' which stands for END
        {
            int buffer_length = str_buffer.length();  // We also find the length of the string so we know how many char to display 
            // Depending upon what came before we update different values
            // To change the reference number we enter R00E, where 00 can be any number up to 99 

            for (int i = buffer_length; i>=0; i--)  // Check the buffer from the end of the data, working backwards
            {
                if (str_buffer[i]=='R')
                {
                    setReferenceFromBuffer(i);
                }

                if(str_buffer[i]=='T')
                {
                    setTimeFromBuffer(i);
                }

                if(str_buffer[i]=='D')
                {
                    setDateFromBuffer(i);
                }           

                if(str_buffer[i]=='S')
                {          
                    setSampleTimeFromBuffer(i);
                }

                if(str_buffer[i]=='O')
                {    
                    VA_StoreNewCurrentOffset();
                }

                if(str_buffer[i]=='V'&&str_buffer[i+1]=='1')
                {
                    String Rstr = str_buffer.substring(i+2,i+5);
                    int value = Rstr.toInt();
                    VA_StoreNewResistor1(value);
                }

                if(str_buffer[i]=='V'&&str_buffer[i+1]=='2')
                {    
                    String Rstr = str_buffer.substring(i+2,i+5);
                    int value = Rstr.toInt();
                    VA_StoreNewResistor2(value);
                }

                if(str_buffer[i]=='I')
                {    
                    String Rstr = str_buffer.substring(i+1,i+4);
                    int value = Rstr.toInt();
                    VA_StoreNewCurrentGain(value);
                }          
            }
            str_buffer="";  // Reset the buffer to be filled again 
        }
    }
}
