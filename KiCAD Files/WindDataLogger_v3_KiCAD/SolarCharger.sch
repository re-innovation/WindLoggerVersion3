EESchema Schematic File Version 2
LIBS:WindLoggerv3_PCB-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:1wire
LIBS:arduino_shieldsNCL
LIBS:atmel-1
LIBS:atmel-2005
LIBS:philips
LIBS:nxp
LIBS:matts_components
LIBS:linear2
LIBS:WindLoggerv3_PCB-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 6
Title ""
Date "25 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 7700 1650
Connection ~ 7050 2050
Wire Wire Line
	6350 2050 7700 2050
Wire Wire Line
	1800 1650 6650 1650
Connection ~ 6350 1650
Wire Wire Line
	7050 1950 7050 2150
Wire Wire Line
	8900 1650 7450 1650
$Comp
L GND #PWR038
U 1 1 5463B707
P 7050 2150
F 0 "#PWR038" H 7050 2150 30  0001 C CNN
F 1 "GND" H 7050 2080 30  0001 C CNN
F 2 "" H 7050 2150 60  0001 C CNN
F 3 "" H 7050 2150 60  0001 C CNN
	1    7050 2150
	1    0    0    -1  
$EndComp
$Comp
L CP1 C13
U 1 1 5463B713
P 7700 1850
F 0 "C13" H 7750 1950 50  0000 L CNN
F 1 "10uF" H 7750 1750 50  0000 L CNN
F 2 "" H 7700 1850 60  0001 C CNN
F 3 "" H 7700 1850 60  0001 C CNN
	1    7700 1850
	1    0    0    -1  
$EndComp
$Comp
L CP1 C12
U 1 1 5463B719
P 6350 1850
F 0 "C12" H 6400 1950 50  0000 L CNN
F 1 "10uF" H 6400 1750 50  0000 L CNN
F 2 "" H 6350 1850 60  0001 C CNN
F 3 "" H 6350 1850 60  0001 C CNN
	1    6350 1850
	1    0    0    -1  
$EndComp
Text HLabel 8900 1650 2    60   Output ~ 0
3V3
Text HLabel 1800 2250 0    60   Input ~ 0
SOLAR+
Text HLabel 1800 4150 0    60   Input ~ 0
GND
Text HLabel 1800 1650 0    60   Input ~ 0
BATT+
$Comp
L LTC3105_MSOP U3
U 1 1 5463CA36
P 4100 3600
F 0 "U3" H 4250 3404 60  0000 C CNN
F 1 "LTC3105_MSOP" H 4000 4250 60  0000 C CNN
F 2 "~" H 4100 3600 60  0000 C CNN
F 3 "~" H 4100 3600 60  0000 C CNN
	1    4100 3600
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR L1
U 1 1 5463CBED
P 4000 2250
F 0 "L1" V 3950 2250 40  0000 C CNN
F 1 "10uH" V 4100 2250 40  0000 C CNN
F 2 "" H 4000 2250 60  0000 C CNN
F 3 "" H 4000 2250 60  0000 C CNN
	1    4000 2250
	0    -1   -1   0   
$EndComp
$Comp
L R R12
U 1 1 5463CBFC
P 5200 3000
F 0 "R12" V 5280 3000 50  0000 C CNN
F 1 "1020k" V 5200 3000 50  0000 C CNN
F 2 "" H 5200 3000 60  0000 C CNN
F 3 "" H 5200 3000 60  0000 C CNN
	1    5200 3000
	1    0    0    -1  
$EndComp
$Comp
L R R13
U 1 1 5463CC0B
P 5200 3750
F 0 "R13" V 5280 3750 50  0000 C CNN
F 1 "332k" V 5200 3750 50  0000 C CNN
F 2 "" H 5200 3750 60  0000 C CNN
F 3 "" H 5200 3750 60  0000 C CNN
	1    5200 3750
	1    0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 5463CC1A
P 3200 3800
F 0 "R11" V 3280 3800 50  0000 C CNN
F 1 "40.2k" V 3200 3800 50  0000 C CNN
F 2 "" H 3200 3800 60  0000 C CNN
F 3 "" H 3200 3800 60  0000 C CNN
	1    3200 3800
	1    0    0    -1  
$EndComp
$Comp
L CP1 C9
U 1 1 5463CC29
P 2800 2550
F 0 "C9" H 2850 2650 50  0000 L CNN
F 1 ">10uf" H 2850 2450 50  0000 L CNN
F 2 "" H 2800 2550 60  0000 C CNN
F 3 "" H 2800 2550 60  0000 C CNN
	1    2800 2550
	1    0    0    -1  
$EndComp
$Comp
L CP1 C11
U 1 1 5463CC38
P 5650 3450
F 0 "C11" H 5700 3550 50  0000 L CNN
F 1 "10uf" H 5700 3350 50  0000 L CNN
F 2 "" H 5650 3450 60  0000 C CNN
F 3 "" H 5650 3450 60  0000 C CNN
	1    5650 3450
	1    0    0    -1  
$EndComp
$Comp
L CP1 C8
U 1 1 5463CC47
P 2350 3600
F 0 "C8" H 2400 3700 50  0000 L CNN
F 1 "4.7uf" H 2400 3500 50  0000 L CNN
F 2 "" H 2350 3600 60  0000 C CNN
F 3 "" H 2350 3600 60  0000 C CNN
	1    2350 3600
	1    0    0    -1  
$EndComp
$Comp
L CP1 C10
U 1 1 5463CC56
P 4800 3800
F 0 "C10" H 4850 3900 50  0000 L CNN
F 1 "1uf" H 4850 3700 50  0000 L CNN
F 2 "" H 4800 3800 60  0000 C CNN
F 3 "" H 4800 3800 60  0000 C CNN
	1    4800 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 4150 5650 4150
Wire Wire Line
	4600 4150 4600 3600
Wire Wire Line
	4600 3600 4500 3600
$Comp
L GND #PWR039
U 1 1 5463CCDE
P 4000 4300
F 0 "#PWR039" H 4000 4300 30  0001 C CNN
F 1 "GND" H 4000 4230 30  0001 C CNN
F 2 "" H 4000 4300 60  0000 C CNN
F 3 "" H 4000 4300 60  0000 C CNN
	1    4000 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 4300 4000 4150
Connection ~ 4000 4150
Wire Wire Line
	3500 3600 3400 3600
Wire Wire Line
	3400 3600 3400 4150
Connection ~ 3400 4150
Wire Wire Line
	4800 4150 4800 4000
Connection ~ 4600 4150
Wire Wire Line
	4500 3100 4800 3100
Wire Wire Line
	4800 3100 4800 3600
Wire Wire Line
	2000 3200 3500 3200
Wire Wire Line
	2350 3200 2350 3400
Wire Wire Line
	2350 3800 2350 4150
Connection ~ 2350 4150
Wire Wire Line
	3500 3500 3200 3500
Wire Wire Line
	3200 3500 3200 3550
Wire Wire Line
	3200 4050 3200 4150
Connection ~ 3200 4150
$Comp
L GND #PWR040
U 1 1 5463CDA3
P 2800 2850
F 0 "#PWR040" H 2800 2850 30  0001 C CNN
F 1 "GND" H 2800 2780 30  0001 C CNN
F 2 "" H 2800 2850 60  0000 C CNN
F 3 "" H 2800 2850 60  0000 C CNN
	1    2800 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2250 3700 2250
Wire Wire Line
	4300 2250 4700 2250
Wire Wire Line
	4700 2250 4700 3400
Wire Wire Line
	4700 3400 4500 3400
Wire Wire Line
	4500 3500 4650 3500
Wire Wire Line
	4650 3500 4650 2400
Wire Wire Line
	4650 2400 3500 2400
Wire Wire Line
	3500 2400 3500 2250
Connection ~ 3500 2250
Wire Wire Line
	2800 2350 2800 2250
Connection ~ 2800 2250
Wire Wire Line
	2800 2850 2800 2750
Wire Wire Line
	4500 3200 4950 3200
Wire Wire Line
	4950 3200 4950 2650
Wire Wire Line
	4950 2650 5650 2650
Wire Wire Line
	5200 2650 5200 2750
Wire Wire Line
	5200 3250 5200 3500
Wire Wire Line
	5200 4150 5200 4000
Connection ~ 4800 4150
Wire Wire Line
	5650 2650 5650 3250
Connection ~ 5200 2650
Wire Wire Line
	5650 4150 5650 3650
Connection ~ 5200 4150
Wire Wire Line
	3500 3300 3050 3300
Wire Wire Line
	3050 3300 3050 4150
Connection ~ 3050 4150
Wire Wire Line
	3500 3100 3300 3100
Wire Wire Line
	3300 3100 3300 2900
Wire Wire Line
	3300 2900 5050 2900
Wire Wire Line
	5050 2900 5050 3350
Wire Wire Line
	5050 3350 5200 3350
Connection ~ 5200 3350
Text GLabel 2000 3200 0    60   Output ~ 0
2.2V
Connection ~ 2350 3200
Wire Wire Line
	5100 1650 5100 2650
Connection ~ 5100 2650
Connection ~ 5100 1650
$Comp
L MCP1703-Matt U4
U 1 1 5467B677
P 7050 1700
F 0 "U4" H 7200 1504 60  0000 C CNN
F 1 "MCP1703" H 7050 1900 60  0000 C CNN
F 2 "~" H 7050 1700 60  0000 C CNN
F 3 "~" H 7050 1700 60  0000 C CNN
	1    7050 1700
	1    0    0    -1  
$EndComp
NoConn ~ 4500 3300
NoConn ~ 3500 3400
$EndSCHEMATC