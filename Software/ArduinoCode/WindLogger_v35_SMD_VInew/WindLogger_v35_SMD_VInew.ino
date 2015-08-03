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
 
 //************ Real Time Clock code*******************
 A PCF8563 RTC is attached to pins:
 ** A4 - SDA (serial data)
 ** A5 - SDC (serial clock)
 ** D2 - Clock out - This gives a 1 second pulse to record the data
 
 RTC PCF8563 code details:
 By Joe Robertson, jmr
 orbitalair@bellsouth.net
 
**********************************************************************************************************/


/************ External Libraries*****************************/
#include <EnableInterrupt.h>
#include <stdlib.h>
#include <Wire.h>          // Required for RTC
#include <Rtc_Pcf8563.h>   // RTC library
#include <SdFat.h>            // SD card library
#include <avr/pgmspace.h>  // Library for putting data into program memory
#include <EEPROM.h>        // For writing values to the EEPROM
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/power.h>

/************ Application Libraries*****************************/
#include "battery.h"
#include "external-volts-amps.h"
#include "wind.h"

/************User variables and hardware allocation**********************************************/

/******* SD CARD*************/
#define SD_CHIP_SELECT_PIN 10 // The SD card Chip Select pin 10
#define SD_CARD_DETECT_PIN 9  // The SD card detect is on pin 6
// The other SD card pins (D11,D12,D13) are all set within SD.h
int s_lastCardDetect = LOW;  // This is the flag for the old reading of the card detect

// SD file system object
SdFat sd;
//Log file
SdFile datafile;  

//File datafile;   // The logging file
String dataString;    // This is the holder for the data as a string. Start as blank
int counter = 0;   // Clue is in the name - its a counter.
long dataCounter = 0;  // This holds the number of seconds since the last data store

/*************Real Time Clock*******/
Rtc_Pcf8563 s_rtc;
#define I2C_RTC 0x51 // 7 bit address (without last bit - look at the datasheet)
#define RTC_INTERRUPT_NUMBER 0  // RTC interrupt - This is pin 2 of ardunio - which is INT0

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

char filename[] = "DXXXXXX.csv";  // This is a holder for the full file name
char deviceID[3]; // A buffer to hold the device ID

long sampleTime = 2;  // This is the time between samples for the DAQ
                      // Sample time is stored in EEPROM in locations 2 & 3

volatile boolean writedataflag = HIGH;  // A flag to tell the code when to write data

// Varibales for writing to EEPROM
int hiByte;      // These are used to store longer variables into EEPROM
int loByte;

// Varibles for 'I'm alive' flash
int aliveFlashCounter = 0;  // This is used to count to give flash every 10 seconds

// These next ints are for the filename conversion
int day_int =0;      // To find the day from the Date for the filename
int day_int1 =0;
int day_int2 =0;
int month_int = 0;
int month_int1 = 0;
int month_int2 = 0;
int year_int = 0;  // Year
int hour_int = 0;
int min_int = 0;
int sec_int = 0;

boolean calibrateFlag = HIGH;  // This flag is lowered if we are in calibrate mode (switch ON)
boolean debugFlag = LOW;    // Set this if you want to be in debugging mode.

//**********STRINGS TO USE****************************
String comma = ",";
String date;        // The stored date from filename creation

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
const char headers[] PROGMEM = "Ref, Date, Time, RPM, Wind, Direction, Batt V, Ext V, Current";  // Headers for the top of the file
const char headersOK[] PROGMEM = "Headers OK";
const char erroropen[] PROGMEM = "Error open";
const char error[] PROGMEM = "ERROR";
const char initialisesd[] PROGMEM = "Init SD";
const char dateerror[] PROGMEM = "Date ERR";
const char reference[] PROGMEM = "The ref is:";
const char noSD[] PROGMEM = "No SD card";

#define MAX_STRING 130      // Sets the maximum length of string probably could be lower
char stringBuffer[MAX_STRING];  // A buffer to hold the string when pulled from program memory

/***************************************************
 *  Name:        RTC
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: I use the CLK_OUT from the RTC to give me exact 1Hz signal
 *               To do this I changed the initialise the RTC with the CLKOUT at 1Hz
 *
 ***************************************************/
void RTC()
{ 
  disableInterrupt(RTC_INTERRUPT_NUMBER);
  dataCounter++;
  aliveFlashCounter++;
  
  if(writedataflag==LOW&&dataCounter>=sampleTime)  // This stops us loosing data if a second is missed
  { 
    // If this interrupt has happened then we want to write data to SD card:
    // Save the pulsecounter value (this will be stored to write to SD card
    storeWindPulseCounts();

    // Reset the DataCounter
    dataCounter = 0;  
    // Set the writedataflag HIGH
    writedataflag=HIGH;
  }
}

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

  enableInterrupt(RTC_INTERRUPT_NUMBER, RTC, RISING);
  
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
  pinMode(SD_CARD_DETECT_PIN,INPUT);  // D9 is the SD card detect on pin 9.
 
  //Set up digital data lines
  pinMode(CALIBRATE_PIN,INPUT_PULLUP);
  
  analogReference(EXTERNAL);  // This should be default, but just to be sure

  // Analog lines
  pinMode(vanePin,INPUT);
  pinMode(voltagePin,INPUT);
  pinMode(current1Pin,INPUT); 

  // Put unused pins to INPUT to try and save power...      
  
  setupRTC();  // Initialise the real time clock  
  
  initialiseSD();    // Inisitalise the SD card   
  createfilename();  // Create the corrct filename (from date)

  // Read the reference number from the EEROM
  deviceID[0] = char(EEPROM.read(0));
  deviceID[1] = char(EEPROM.read(1));
  
  // Read in the sample time from EEPROM
  hiByte = EEPROM.read(2);
  loByte = EEPROM.read(3);
  sampleTime = (hiByte << 8)+loByte;  // Get the sensor calibrate value 
  
  // Read the Current Voltage Offset from the EEROM
  setCurrentOffset(EEPROM.read(4),EEPROM.read(5));

  setVoltageDivider(
    EEPROM.read(6),
    EEPROM.read(7),
    EEPROM.read(8),
    EEPROM.read(9)
  );

  // read the current gain value
  setCurrentGain(EEPROM.read(10), EEPROM.read(11));
  
  // Interrupt for the 1Hz signal from the RTC
  enableInterrupt(RTC_INTERRUPT_NUMBER, RTC, RISING); 
  // Attach interrupts for the pulse counting
  setupWindPulseInterrupts();
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
   convertWindDirection(analogRead(vanePin));    // Run this every second. It increments the windDirectionArray 
 
  if(aliveFlashCounter>=10)
  {
    // Flash the LED every 10 seconds to show alive
    pinMode(RED_LED_PIN,OUTPUT);    // Set LED to be an output LED 
    digitalWrite(RED_LED_PIN, HIGH);   // set the LED ON
    delay(5);
    digitalWrite(RED_LED_PIN, LOW);   // set the LED OFF 
    aliveFlashCounter=0;  // Reset the counter 
  }
  
  if(writedataflag==HIGH)
  {  
    pinMode(RED_LED_PIN,OUTPUT);    // Set LED to be an output LED 
    digitalWrite(RED_LED_PIN, HIGH);   // set the LED ON

    // *********** WIND SPEED ******************************************
    // Want to get the number of pulses and average into the sample time
    // This gives us the average wind speed
    // pulsecounterold holds the value of pulses.
    // This can be converted into the wind speed using the time and 
    // the pulse-wind speed characterisitic of the anemometer.
    // Do this as post processing - pulse count is most important.
    
    // *********** WIND DIRECTION **************************************
    // This can be checked every second and an average used
    analyseWindDirection();

//    // *********** TEMPERATURE *****************************************
//    // Two versions of this - either with thermistor or I2C sensor (if connected)
//    // Thermistor version
//    // Get the temperature readings and store to variables
//    TempC = Temperature(thermistor,T_CELSIUS,GT_Thermistor_10k,1000.0f);
//    dtostrf(TempC,2,2,TempCStr);  // Convert the temperature value (double) into a string
//    
////    if(debugFlag==HIGH)
////    {
////      Serial.print("Therm: ");
////      Serial.println(TempCStr);  
////    }   

    updateBatteryVoltage();
    
    // *********** EXTERNAL VOLTAGE ***************************************
    // From Vcc-680k--46k-GND potential divider
    updateExternalVoltage();
    
    // ********** EXTERNAL CURRENTS **************************************
    // Measured using a hall effect current sensor
    // Either using a ACS*** or a LEM HTFS 200-P
    // Comment out whichever you are not using
 
    updateExternalCurrent();
  
    // ******** put this data into a file ********************************
    // ****** Check filename *********************************************
    // Each day we want to write a new file.
    // Compare date with previous stored date, every second
    newdate = String(s_rtc.formatDate(RTCC_DATE_WORLD));  
    if(newdate != date)
    {
       // If date has changed then create a new file
       createfilename();  // Create the corrct filename (from date)
    }    

    // ********* Create string of data **************************
    dataString =  String(deviceID[0]); 
    dataString += deviceID[1];  // Reference
    dataString += comma;
    dataString += newdate;  // Date
    dataString += comma;
    dataString += String(s_rtc.formatTime()); // Time
    dataString += comma;
    dataString += getPulseCountStr(0);
    dataString += comma;
    dataString += getPulseCountStr(1);
    dataString += comma;
    dataString += getWindDirectionStr(); // Wind direction
//    dataString += comma;
//    dataString += TempCStr; // Temperature
    dataString += comma;
    dataString += getBatteryVoltageStr();  // Battery voltage  
    dataString += comma;
    dataString += getExternalVoltageStr();  // Current 1 reading
    dataString += comma;
    dataString += getExternalCurrentStr();  // Current 2 reading
 
    
    // ************** Write it to the SD card *************
    // This depends upon the card detect.
    // If card is there then write to the file
    // If card has recently been inserted then initialise the card/filenames
    // If card is not there then flash LEDs

    if(digitalRead(SD_CARD_DETECT_PIN)==LOW&&s_lastCardDetect==HIGH)
    {
      delay(100);  // Wait for switch to settle down.
      // There was no card previously so re-initialise and re-check the filename
      initialiseSD();
      createfilename();
    }
    if(digitalRead(SD_CARD_DETECT_PIN)==LOW&&s_lastCardDetect==LOW)
    {
      //Ensure that there is a card present)
      // We then write the data to the SD card here:
      writetoSD();
    }
    else
    {
       // print to the serial port too:
       Serial.println(dataString);
       Serial.println(getString(noSD));
    }   
    s_lastCardDetect = digitalRead(SD_CARD_DETECT_PIN);  // Store the old value of the card detect
    
    // Finish up write routine here:    
    digitalWrite(RED_LED_PIN, LOW);   // set the LED OFF 
    pinMode(RED_LED_PIN,INPUT);    // Set LED to be an INPUT - saves power   
    writedataflag=LOW;
    Serial.flush();    // Force out the end of the serial data
  }
  
  // Want to check the SD card every second
  if(digitalRead(SD_CARD_DETECT_PIN)==HIGH)
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
    
  if(debugFlag==HIGH)
  {
    // DEBUGGING ONLY........
    Serial.print("Anemometer1: ");
    Serial.println(getLivePulseCount(0), DEC);
    Serial.print("Anemometer2: ");
    Serial.println(getLivePulseCount(1), DEC);
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

    writedataflag=HIGH;  // Set the write data flag high
  }
  else
  {     
    enableInterrupt(RTC_INTERRUPT_NUMBER, RTC, RISING); 
    enterSleep();     
  }
  
}

// Set Up RTC routine
void setupRTC()
{
    // This section configures the RTC to have a 1Hz output.
  // Its a bit strange as first we read the data from the RTC
  // Then we load it back again but including the correct second flag  
  s_rtc.formatDate(RTCC_DATE_WORLD);
  s_rtc.formatTime();
  
  year_int = s_rtc.getYear();
  day_int = s_rtc.getDay();
  month_int = s_rtc.getMonth();  
  hour_int = s_rtc.getHour();
  min_int = s_rtc.getMinute();
  sec_int = s_rtc.getSecond(); 
  
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
  Wire.beginTransmission(I2C_RTC); // Select RTC
  Wire.write(0);        // Start address
  Wire.write(0);     // Control and status 1
  Wire.write(0);     // Control and status 2
  Wire.write(DecToBcd(sec_int));     // Second
  Wire.write(DecToBcd(min_int));    // Minute
  Wire.write(DecToBcd(hour_int));    // Hour
  Wire.write(DecToBcd(day_int));    // Day
  Wire.write(DecToBcd(2));    // Weekday
  Wire.write(DecToBcd(month_int));     // Month (with century bit = 0)
  Wire.write(DecToBcd(year_int));    // Year
  Wire.write(0b10000000);    // Minute alarm (and alarm disabled)
  Wire.write(0b10000000);    // Hour alarm (and alarm disabled)
  Wire.write(0b10000000);    // Day alarm (and alarm disabled)
  Wire.write(0b10000000);    // Weekday alarm (and alarm disabled)
  Wire.write(0b10000011);     // Output clock frequency enabled (1 Hz) ***THIS IS THE IMPORTANT LINE**
  Wire.write(0);     // Timer (countdown) disabled
  Wire.write(0);     // Timer value
  Wire.endTransmission();
}

// Converts a decimal to BCD (binary coded decimal)
byte DecToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}

//*********** FUNCTION TO INITIALISE THE SD CARD***************
void initialiseSD()
{
  // Initialize the SD card at SPI_HALF_SPEED to avoid bus errors 
  // We use SPI_HALF_SPEED here as I am using resistor level shifters.
  //if (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) sd.initErrorHalt();
  
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SD_CHIP_SELECT_PIN, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) {
//    if(debugFlag==HIGH)
//    {
//      Serial.println("FAIL");
//    }
    // don't do anything more:
    // Want to turn on an ERROR LED here
    return;
  }
  else
  {
    if(debugFlag==HIGH)
    {
      Serial.println(getString(initialisesd));
    }
  }
}

// *********FUNCTION TO SORT OUT THE FILENAME**************
void createfilename()
{
  // Check there is a file created with the date in the title
  // If it does then create a new one with the new name
  // The name is created from:
  // DMMDDYY.CSV, where YY is the year MM is the month, DD is the day
  // You must add on the '0' to convert to ASCII
  
  date = String(s_rtc.formatDate());
  day_int = s_rtc.getDay();  // Get the actual day from the RTC
  month_int = s_rtc.getMonth();  // Get the month
  day_int1 = day_int/10;    // Find the first part of the integer
  day_int2 = day_int%10;    // Find the second part of the integer
  month_int1 = month_int/10;    // Find the first part of the integer
  month_int2 = month_int%10;    // Find the second part of the integer
  filename[1]=(year_int/10) + '0';  // Convert from int to ascii
  filename[2]=(year_int%10) + '0';  // Convert from int to ascii 
  filename[3]= month_int1 + '0';  // Convert from int to ascii
  filename[4]= month_int2 + '0';  // Convert from int to ascii   
  filename[5]= day_int1 + '0';  // Convert from int to ascii
  filename[6]= day_int2 + '0';  // Convert from int to ascii 
  
  if(debugFlag==HIGH)
  {
    Serial.println(filename);
  }
  
  if(!sd.exists(filename))
  {
    // open the file for write at end like the Native SD library
    if (!datafile.open(filename, O_RDWR | O_CREAT | O_AT_END)) 
    {
//      if(debugFlag==HIGH)
//      {
//        Serial.println(getString(erroropen));
//      }
    }
    // if the file opened okay, write to it:
    datafile.println(getString(headers));
    // close the file:
    datafile.sync();
//    if(debugFlag==HIGH)
//    {
//      Serial.println(getString(headersOK));
//    }
  } 
  else
  {
//    if(debugFlag==HIGH)
//    {
//      Serial.println("Filename exists");
//    }
  }
  
}

// This routine writes the dataString to the SD card
void writetoSD()
{
  datafile.open(filename, O_RDWR | O_CREAT | O_AT_END);    // Open the correct file
  // if the file is available, write to it:
  if (sd.exists(filename)) {
    datafile.println(dataString);
    datafile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
//    if(debugFlag==HIGH)
//    {
//      Serial.println(getString(erroropen));
//    }
  }
}

// Get a string from program memory
// This routine pulls the string stored in program memory so we can use it
// It is temporaily stored in the stringBuffer
char* getString(const char* str) {
	strcpy_P(stringBuffer, (char*)str);
	return stringBuffer;
}

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
              deviceID[0]=str_buffer[i+1];
              deviceID[1]=str_buffer[i+2];
              Serial.print(getString(reference));
              Serial.print(deviceID[0]);
              Serial.println(deviceID[1]);
              EEPROM.write(0,deviceID[0]);
              EEPROM.write(1,deviceID[1]);
              initialiseSD();
              createfilename();
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
              s_rtc.setTime(hour, minute, second);

              initialiseSD();
              createfilename();
              
              Serial.println(String(s_rtc.formatTime())); // Time
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
           
              //day, weekday, month, century(1=1900, 0=2000), year(0-99)
              s_rtc.setDate(day, 3, month, 0, year);
              
              initialiseSD();
              createfilename();
              
              Serial.println(String(s_rtc.formatDate(RTCC_DATE_WORLD)));
          }           
          if(str_buffer[i]=='S')
          {          
              // In this case we have changed the sample time, so UPDATE and store to EEPROM
              // Data will be in the form of 5 x chars, signifying XXXXX, a value from 00001 to 99999 seconds
              
              sampleTime = atol(&str_buffer[i+1]);  // Convert the string to a long int
              
              EEPROM.write(2, sampleTime >> 8);    // Do this seperately
              EEPROM.write(3, sampleTime & 0xff);
              Serial.print("Sample Time:");
              Serial.println(sampleTime);
              
              dataCounter=0;  // Reset the data counter to start counting again.
          }
          
          if(str_buffer[i]=='O')
          {    
            // ******** Recalibrate the offset *******************
            // We want to measure the offset for the current sensor here. 
            // This will be changed to a calibrate function.
            // When switched on the unit should not have any current through the sensor or this
            // will read incorrectly.
            storeNewCurrentOffset();
          }   
          if(str_buffer[i]=='V'&&str_buffer[i+1]=='1')
          {
            // ******** Change resistor r1 ******************
            String Rstr = str_buffer.substring(i+2,i+5);
            int value = Rstr.toInt();
            storeNewResistor1(value);
          }        
          if(str_buffer[i]=='V'&&str_buffer[i+1]=='2')
          {    
            // ******** Change resistor r1 ******************
            String Rstr = str_buffer.substring(i+2,i+5);
            int value = Rstr.toInt();
            storeNewResistor2(value);
          } 
          if(str_buffer[i]=='I')
          {    
            // ******** Change Current gain value (mV) ******************
            String Rstr = str_buffer.substring(i+1,i+4);
            int value = Rstr.toInt();
            storeNewCurrentGain(value);
          } 
          
        }
        str_buffer="";  // Reset the buffer to be filled again 
      }
    }
  }
}
