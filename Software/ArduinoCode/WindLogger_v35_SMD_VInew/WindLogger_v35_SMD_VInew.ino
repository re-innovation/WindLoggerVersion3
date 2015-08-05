/********************************************************
/****** Wind Logger based upon the:  ********************
/****** DataDuino - Arduino DAQ UNIT ********************
/****** by Matt Little **********************************
/****** Date: 29/1/2014 *********************************
/****** info@re-innovation.co.uk ************************
/****** www.re-innovation.co.uk *************************
/********************************************************

  See www.re-innovation.co.uk for information and construction details
  
  This is sample code for the DataDuino.

/*************Details of Code*****************************

  The DataDuino is an Arduino based SD card datalogger.
  A PCF8563 Real Time Clock is used to timestamp the data.
  
  The pin assisgnments are as follows:
  D0 - Rx Serial Data
  D1 - Tx Serial Data
  D2 - RTC interrupt (1 second pulses)
  D3 - Anemometer 1 (pulse interrupt) (& Power to External Unit - eg GSM)
  D4 - LED DATA output
  D5 - Anemometer 2 (pulse interrupt)
  D6 - Calibrate Switch (pull LOW to set)
  D7 - Rx_GSM
  D8 - Tx_GSM
  D9 - Card Detect (SD)
  D10 - Chip Select - SD card
  D11 - MOSI SD card
  D12 - MISO SD card
  D13 - SPI CLock SD card
  
  A0 - Wind Vane measurement with 10k pull-down
  A1 - Internal battery voltage measurement (from potential divider)
  A2 - External voltage measurement (with potential divider) 0-40V DC
  A3 - Current measurement
  A4 - SDA - I2C connection to RTC
  A5 - SCK - I2C connection to RTC
  
  Counts pulses from a sensor (such as a anemometer or flow sensor)
  These are pulses are averaged into a wind speed.
  We also keep a record of the maximum in the averaging period.
  For turbulence measurement 
  
  Each logger has a reference (user adjustable).
  
  Data is written to a .csv file created on an SD card.
  A new file is created each day. If file alreay present then data is appended.
  The file name is created from the reference number and the date in the format:
  DXXXXXX.csv, where DXXXXXX is the date in the format DDMMYY. 
  
  Data is stored with human readable headers:

  // This will be added to a seperate 'calibrate' mode
  When in Calibrate mode:
  You can adjust the parameters of the device using serial commands. These parameters are stored in EEPROM.
  These are:
  "T??????E"
  This will change the time to HHMMSS
  "??????E"
  This will change the date to DDMMYY
  "S?????E"
  This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data.
  The minimum is 1 second data. The maximum is 99999 seconds
  "R??E"
  This will change the reference to ??. 
  "OE"
  This will take the current reading and write it to the current offset.
  "V1???E"
  "V2???E"
  These select the two resistor values from the resistor potential divider.
  Values are in k Ohm.
  R1 is the top resistor, R2 is the lower resistor.
  "I???E"
  This sets the current gain value, in mV/A
 
  
  // Addedd Interrupt code from here:
  // PinChangeIntExample, version 1.1 Sun Jan 15 06:24:19 CST 2012
  // See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki 
  // for more information.
 
  Updates: 
  30/10/12 Code written - Matt Little
  31/10/12 writing code description and sorting out new sample timings - Matt Little
  19/9/13  Getting code to work on Arduino 1.0.5 - Matt Little
  5/2/14   Adding wind logging sections - Matt Little
  5/2/14   Sorting out sleep modes - Matt Little
  5/2/14   Adding HIH humidity/temperature sensor - Matt Little
  5/2/14   Sorting out Card Detect when card removed - Matt Little
  10/2/14  Sorting out card re-enter issue - Matt Little
  17/6/14  Adding pull-up enable for calibrate pin.- Matt Little
  17/6/14  Removing humidity sensor routines. Not needed -  Matt Little
  17/6/14  Adding debounce timer for anemometer pulses - Matt Little
  17/6/14  Adding Direction vane input - Matt Little
  10/7/14  Adding additional Anemometer input - Matt Little
  10/7/14  Needs additional interrupt pin added - Matt Little
  13/8/14  Added wind direction data - Matt Little
  15/8/14  Added 'I'm Alive' LED flash every 5 seconds - Matt Little
  11/3/15  Added string output when NO SD and in Calibrate Mode
  18/3/15  Changed code for V and I SMD design - Matt Little
  20/3/15  More work on V and I code. SMD design - Matt Little
  15/4/15  Re-doing code for new hardware - Matt Little
  21/4/15  Adding voltage and current calibration factors in serial - Matt Little
  
  TO DO
  Sort out Voltage conversion (via serial) - implemented - TEST
  Sort out Current conversion (via serial) - implemented - TEST
  Sort out RPM sensor (want RPM value, so convert correctly)
  Sort out maximum wind speed in time period
 
 //*********SD CARD DETAILS***************************	
 The SD card circuit:
 SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
 ** Card detect - pin 9
 
**********************************************************************************************************/


/************ External Libraries*****************************/
#include <stdlib.h>
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>
#include <Wire.h>          // Required for RTC
#include <Rtc_Pcf8563.h>   // RTC library
#include <SdFat.h>            // SD card library
#include <avr/pgmspace.h>  // Library for putting data into program memory
#include <EEPROM.h>        // For writing values to the EEPROM
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/power.h>

/************ Application Libraries*****************************/
#include "eeprom_storage.h"
#include "battery.h"
#include "external-volts-amps.h"
#include "wind.h"
#include "rtc.h"
#include "sd.h"
#include "utility.h"
#include "app.h"

/************User variables and hardware allocation**********************************************/

int counter = 0;   // Clue is in the name - its a counter.

/********* I/O Pins *************/
#define RED_LED_PIN 4      // The output led is on pin 4
#define CALIBRATE_PIN 6   // This controls if we are in serial calibrate mode or not

///********** Thermistor Data Storage ************/
//#define thermistor A0  // This is the analog pin for the thermistor
//float TempC = 0;  // This holds the converted value of temperature
//char TempCStr[6];  // A string buffer to hold the converted string
//// enumarating 3 major temperature scales
//enum {
//  T_KELVIN=0,
//  T_CELSIUS,
//  T_FAHRENHEIT
//};
//// Manufacturer data for episco k164 10k thermistor
//// simply delete this if you don't need it
//// or use this idea to define your own thermistors
////#define EPISCO_K164_10k 4300.0f,298.15f,10000.0f  // B,T0,R0
//#define GT_Thermistor_10k 4126.0f,298.15f,10000.0f  // B,T0,R0
////#define Vishay_Thermistor_47k 4090.0f,298.15f,47000.0f  // B,T0,R0
/////********* Thermistor Temperature sensor****************/
//float temp;        // Temporary store for float

// Lets use a dallas 1-wire sensotr, to save an analog value.


// ****** Serial Data Read***********
// Variables for the serial data read
char inByte;         // incoming serial char
String str_buffer = "";  // This is the holder for the string which we will display

//********Variables for the Filename*******************

// Varibales for writing to EEPROM
int hiByte;      // These are used to store longer variables into EEPROM
int loByte;

// Varibles for 'I'm alive' flash
int aliveFlashCounter = 0;  // This is used to count to give flash every 10 seconds

bool calibrateFlag = HIGH;  // This flag is lowered if we are in calibrate mode (switch ON)
bool s_debugFlag = LOW;    // Set this if you want to be in debugging mode.

//**********STRINGS TO USE****************************

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
const char headersOK[] PROGMEM = "Headers OK";
const char erroropen[] PROGMEM = "Error open";
const char error[] PROGMEM = "ERROR";
const char dateerror[] PROGMEM = "Date ERR";
const char reference[] PROGMEM = "The ref is:";

/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  RTC_EnableInterrupt();
  
  sleep_enable();
   
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  
  byte old_ADCSRA = ADCSRA;  // Store the old value to re-enable 
  // disable ADC
  ADCSRA = 0;

  byte old_PRR = PRR;  // Store previous version on PRR
  // turn off various modules
  PRR = 0b11111111;
  
  sleep_cpu();
  /* The program will continue from here. */
  /************* ASLEEP *******************/
  
  // ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ //
  
  /************** WOKEN UP ***************/
  /* First thing to do is disable sleep. */
  sleep_disable();
  
  // turn ON various modules USART and ADC
  PRR = old_PRR;  
  
  // enable ADC
  ADCSRA = old_ADCSRA;  
}

/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the Arduino.           
 *
 ***************************************************/
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  
  //******Real Time Clock Set - up********
  // A4 and A5 are used as I2C interface.
  // D2 is connected to CLK OUT from RTC. This triggers an interrupt to take data
  // We need to enable pull up resistors
  pinMode(A4, INPUT);           // set pin to input
  digitalWrite(A4, HIGH);       // turn on pullup resistors
  pinMode(A5, INPUT);           // set pin to input
  digitalWrite(A5, HIGH);       // turn on pullup resistors
  pinMode(2,INPUT);    // Set D2 to be an input for the RTC CLK-OUT   

  //initialise the real time clock

  //initialisetemp();  // Initialise the temperature sensors
  pinMode(RED_LED_PIN,OUTPUT);    // Set D4 to be an output LED
  
  SD_Setup();
  
  //Set up digital data lines
  pinMode(CALIBRATE_PIN,INPUT_PULLUP);
  
  analogReference(EXTERNAL);  // This should be default, but just to be sure

  // Analog lines
  pinMode(VANE_PIN,INPUT);
  pinMode(VOLTAGE_PIN,INPUT);
  pinMode(CURRENT_1_PIN,INPUT); 

  // Put unused pins to INPUT to try and save power...      
  
  RTC_Setup();  // Initialise the real time clock  
  
  SD_CreateFileForToday();  // Create the corrct filename (from date)

  // Read the reference number from the EEPROM
  char deviceID[2];
  EEPROM_GetDeviceID(deviceID);
  SD_SetDeviceID(deviceID);
  
  // Read in the sample time from EEPROM
  SD_SetSampleTime( EEPROM_GetSampleTime() );
  
  // Read the Current Voltage Offset from the EEROM
  VA_SetCurrentOffset( EEPROM_GetCurrentOffset() );

  VA_SetVoltageDivider(
    EEPROM_GetR1(),
    EEPROM_GetR2()
  );

  // read the current gain value
  VA_SetCurrentGain( EEPROM_GetCurrentGain() );
  
  // Interrupt for the 1Hz signal from the RTC
  RTC_EnableInterrupt();

  // Attach interrupts for the pulse counting
  WIND_SetupWindPulseInterrupts();
}

/***************************************************
 *  Name:        main loop
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{

  String newdate;     // The new date, read every time 

  // *********** WIND DIRECTION **************************************  
  // Want to measure the wind direction every second to give good direction analysis
  // This can be checked every second and an average used
  WIND_ConvertWindDirection(analogRead(VANE_PIN));    // Run this every second. It increments the windDirectionArray 
 
  if(aliveFlashCounter>=10)
  {
    // Flash the LED every 10 seconds to show alive
    pinMode(RED_LED_PIN,OUTPUT);    // Set LED to be an output LED 
    digitalWrite(RED_LED_PIN, HIGH);   // set the LED ON
    delay(5);
    digitalWrite(RED_LED_PIN, LOW);   // set the LED OFF 
    aliveFlashCounter=0;  // Reset the counter 
  }
  
  if(SD_WriteIsPending())
  {  
    pinMode(RED_LED_PIN,OUTPUT);    // Set LED to be an output LED 
    digitalWrite(RED_LED_PIN, HIGH);   // set the LED ON

    SD_WriteData();
    
    // Finish up write routine here:    
    digitalWrite(RED_LED_PIN, LOW);   // set the LED OFF 
    pinMode(RED_LED_PIN,INPUT);    // Set LED to be an INPUT - saves power   
    Serial.flush();    // Force out the end of the serial data
  }
  
  // Want to check the SD card every second
  if(!SD_CardIsPresent())
  {
    pinMode(RED_LED_PIN,OUTPUT);    // Set LED to be an output LED 
    // This meands there is no card present so flash the LED every second
    for(int x=0;x<=5;x++)
    {
      digitalWrite(RED_LED_PIN, HIGH);   // set the LED ON
      delay(5);
      digitalWrite(RED_LED_PIN, LOW);   // set the LED ON
      delay(50);     
    }
  } 
    
  if(s_debugFlag==HIGH)
  {
    // DEBUGGING ONLY........
    Serial.print("Anemometer1: ");
    Serial.println(WIND_GetLivePulseCount(0), DEC);
    Serial.print("Anemometer2: ");
    Serial.println(WIND_GetLivePulseCount(1), DEC);
  }
  
  // A Switch on D7 will set if the unit is in serial adjust mode or not  
  //calibrateFlag = digitalRead(calibrate);  
  
  if(digitalRead(CALIBRATE_PIN)== HIGH)
  {    
    // We ARE in calibrate mode
    Serial.println("Calibrate");    
    getData();
    delay(500);  // Some time to read data
    Serial.flush();    // Force out the end of the serial data 

    SD_ForcePendingWrite();
  }
  else
  {     
    RTC_EnableInterrupt(); 
    enterSleep();     
  }  
}

// *********FUNCTION TO SORT OUT THE FILENAME**************

//// Temperature function outputs float , the actual
//// temperature
//// Temperature function inputs
//// 1.AnalogInputNumber - analog input to read from
//// 2.OuputUnit - output in celsius, kelvin or fahrenheit
//// 3.Thermistor B parameter - found in datasheet
//// 4.Manufacturer T0 parameter - found in datasheet (kelvin)
//// 5. Manufacturer R0 parameter - found in datasheet (ohms)
//// 6. Your balance resistor resistance in ohms  
//
//float Temperature(int AnalogInputNumber,int OutputUnit,float B,float T0,float R0,float R_Balance)
//{
//  float R,T,data;
//
//  //R=1024.0f*R_Balance/float(analogRead(AnalogInputNumber))-R_Balance;
//  
//  // Changes as using thermistor to ground:
//  data = float(analogRead(AnalogInputNumber));
//  R=(data*R_Balance)/(1024.0f-data);
//  
//  T=1.0f/(1.0f/T0+(1.0f/B)*log(R/R0));
//
//  switch(OutputUnit) {
//    case T_CELSIUS :
//      T-=273.15f;
//    break;
//    case T_FAHRENHEIT :
//      T=9.0f*(T-273.15f)/5.0f+32.0f;
//    break;
//    default:
//    break;
//  };
//
//  return T;
//}


// **********************GET DATA SUBROUTINE*****************************************
// This sub-routine picks up and serial string sent to the device and sorts out a power string if there is one
// All values are global, hence nothing is sent/returned

void getData()
{
    // **********GET DATA*******************************************
  // We want to find the bit of interesting data in the serial data stream
  // If we write H then house number then the code will update the house number in EEPROM
  // **** aslo need to write code to update RTC
  
  for(int i = 0; i<10;i++)  // This helps us just take a 'chunk' of data so does not fill up serial buffer
  {
    // get incoming bytes:
    if (Serial.available() > 0) 
    {
     inByte = Serial.read(); 
     str_buffer+=inByte;
    
     if(inByte=='E')    // We read everything up to the byte 'E' which stands for END
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

void APP_SecondTick()
{
  aliveFlashCounter++;  
}

bool APP_InDebugMode()
{
  return s_debugFlag;
}