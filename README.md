# lan_nctu

## Midi Command
### Control Change Map
* Control #0 ~ Control #3 are used to change program.
* Control #4 is for the mapped value of accelerometer measurement.
* Control #5 ~ Control #7 are for buttons.
* Control #8 is for the 3-way switch.

## Data Format between 2 Sparkfun Pro Micro
### Each data is a 16-bit integer,
* The first 8 bits from the high bits represent the shaking time.
	* Bit 7 is currently not in used.
	* Bit 0~6 is an interger mapping from (20 ms ~ 2000 ms) to (0 ~ 127).
	* The value is decided by the define part of [lan.ino](https://github.com/showaykerker/lan_nctu/blob/master/lan/lan.ino).
* The last 8 bits represent the button and switch states.
	* Bit 0 from the low bits represent the state of button 1.
	* Bit 1 from the low bits represent the state of button 2.
	* Bit 2 from the low bits represent the state of button 3.
	* Bit 3 and bit 4 from the low bits represent the state of the 2-way switch.
		* only 3 values are used, which are 00, 01 and 10.
	* Bit 5 and bit 6 from the low bits represent the state of the top switch (program).
	* The 7 bit is currently not in used.

## Calculations of Imu.
* The calculation of imu used only accelerometer.
	* In [imu.hpp](https://github.com/showaykerker/lan_nctu/blob/master/lan/imu.hpp)
		* First, it calculates the root-mean-square value of y-axis reading and z-axis reading since the x-axis is parrallel to the lan itself.
		* Second, it finds 2 time neighbhoring steps whose values are across 0 as critical time steps.
		* Third, it calculates the length of the 2 most recent critical time steps.
	* In [lan.ino](https://github.com/showaykerker/lan_nctu/blob/master/lan/lan.ino)
		* The length is mapped from 2 (*10ms) ~ 200 (*10ms) to 0 ~ 127.
		* Then a simple low pass filter is applied.
* The Result can be shown below.
	* ![image](https://github.com/showaykerker/lan_nctu/blob/master/asset/delay_calculation.png)
	* The blue line and red line represents the yz-value reading.
	* The green line is the length of the 2 most recent critical time steps.
	* The orange line is the filtered value.
