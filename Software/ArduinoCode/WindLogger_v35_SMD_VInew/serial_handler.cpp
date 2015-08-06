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

const char reference[] PROGMEM = "The ref is:";

void SERIAL_GetCalibrationData()
{
    // **********GET DATA*******************************************
  // We want to find the bit of interesting data in the serial data stream
  // If we write H then house number then the code will update the house number in EEPROM
  // **** aslo need to write code to update RTC
  
  char next_byte;
  String str_buffer = "";  // This is the holder for the string which we will display

  for(int i = 0; i<10;i++)  // This helps us just take a 'chunk' of data so does not fill up serial buffer
  {
    // get incoming bytes:
    if (Serial.available() > 0) 
    {
     next_byte = Serial.read(); 
     str_buffer += next_byte;
    
     if(next_byte=='E')    // We read everything up to the byte 'E' which stands for END
     {
       int buffer_length = str_buffer.length();  // We also find the length of the string so we know how many char to display 
       // Depending upon what came before we update different values
       // To change the reference number we enter R00E, where 00 can be any number up to 99 

        for(int i = buffer_length; i>=0; i--)  // Check the buffer from the end of the data, working backwards
        {
          if(str_buffer[i]=='R')
          {
              // In this case we have changed the house number, so UPDATE and store in EEPROM
              SD_SetDeviceID(&str_buffer[i+1]);
              EEPROM_SetDeviceID(&str_buffer[i+1]);

              Serial.print(GetString(reference));
              Serial.print(str_buffer[i+1]);
              Serial.println(str_buffer[i+2]);
              SD_CreateFileForToday();
          }          
          if(str_buffer[i]=='T')
          {
              // In this case we have changed the TIME, so UPDATE and store to RTC
              // The time is in the format  HHMMSS
              
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
          if(str_buffer[i]=='D')
          {
              // In this case we have changed the DATE, so UPDATE and store to RTC
              // The time is in the format  DDMMYY
              
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
          if(str_buffer[i]=='S')
          {          
              // In this case we have changed the sample time, so UPDATE and store to EEPROM
              // Data will be in the form of 5 x chars, signifying XXXXX, a value from 00001 to 99999 seconds
              
              long sampleTime = atol(&str_buffer[i+1]);  // Convert the string to a long int
              
              EEPROM_SetSampleTime((uint16_t)sampleTime);              
              
              Serial.print("Sample Time:");
              Serial.println(sampleTime);
              
              SD_ResetCounter();
          }
          
          if(str_buffer[i]=='O')
          {    
            // ******** Recalibrate the offset *******************
            // We want to measure the offset for the current sensor here. 
            // This will be changed to a calibrate function.
            // When switched on the unit should not have any current through the sensor or this
            // will read incorrectly.
            VA_StoreNewCurrentOffset();
          }   
          if(str_buffer[i]=='V'&&str_buffer[i+1]=='1')
          {
            // ******** Change resistor r1 ******************
            String Rstr = str_buffer.substring(i+2,i+5);
            int value = Rstr.toInt();
            VA_StoreNewResistor1(value);
          }        
          if(str_buffer[i]=='V'&&str_buffer[i+1]=='2')
          {    
            // ******** Change resistor r1 ******************
            String Rstr = str_buffer.substring(i+2,i+5);
            int value = Rstr.toInt();
            VA_StoreNewResistor2(value);
          } 
          if(str_buffer[i]=='I')
          {    
            // ******** Change Current gain value (mV) ******************
            String Rstr = str_buffer.substring(i+1,i+4);
            int value = Rstr.toInt();
            VA_StoreNewCurrentGain(value);
          } 
          
        }
        str_buffer="";  // Reset the buffer to be filled again 
      }
    }
  }
}
