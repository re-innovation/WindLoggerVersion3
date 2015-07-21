# WindLoggerVersion3
A datalogger with inputs for measuring wind speed, wind direction, along with current and voltage (with additional sensors)

#Arduino code

  See www.re-innovation.co.uk for information and construction details
  
  This is sample code for the DataDuino.

##Details of code

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

  This will be added to a seperate 'calibrate' mode
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
  
  "V1???E" &  "V2???E"
  
  These select the two resistor values from the resistor potential divider.
  Values are in k Ohm.
  R1 is the top resistor, R2 is the lower resistor.
  
  "I???E"
  
  This sets the current gain value, in mV/A
