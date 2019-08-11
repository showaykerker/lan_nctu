# 阿蘭

## 簡介
深受美秀集團自製樂器「台八線」啟發，揉合心中對於電子訊號與手作的熱愛後誕生。阿蘭的演奏方式為再譯人聽音樂的肢體動作，應用感測裝置、Arduino、開源合成器VCV Rack、圈型LED燈來創造一種新型態的樂器演奏體驗。阿蘭的本體由透明壓克力管手工打造，管身上方分別有可以演奏八度音的音高按鈕，以及三段式的音色控制鈕；管身前端為可以變化背景Sequencer的切換器，為阿蘭帶來獨立演奏或是樂團中配器的兩種角色。手作電子樂器——阿蘭隸屬於交大「學生自主學習社群」計畫創新實作研究類，並會持續進化成易用性與酷炫性兼具的自製樂器。

## Demo 影片
[![Lannctu Demo Video](https://img.youtube.com/vi/T7qD4VoRuG8/maxresdefault.jpg)](https://youtu.be/T7qD4VoRuG8)

## Technical Details

### Midi Map
#### Reciever to VCV
* [CC] Control #0 ~ Control #3 are used to change program. (top switch)
* [CC] Control #4 is for the mapped value of accelerometer measurement.
* [CC] Control #8 ~ Control #10 are for buttons.
* [CC] Control #12 ~ Control #14 is for the 2 way switch.
* [CV] Buttons are used to perform different note with different combinations. (val2note in `reciever.ino`)
#### VCV to Reciever
* [CV] VCV pass a value to reciever through channel #1 MW.


### Data Format between 2 Sparkfun Pro Micro
#### lan to Reciever
* Each data is a 16-bit integer,
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
		
#### Reciever to lan
* The reciever read BPM from VCV Rack, than pass this value to lan.
	* The relation between BPM and read value is a exponential function. [data here](https://github.com/showaykerker/lan_nctu/blob/master/CV_to_BPM_Calculation.xlsx)
	* Each data contains only 1 16-bit integer which represents BPM ranging from 60 ~ 540.
	* Note that the calculated BPM has an error to the real value caused by the resolution of midi control level.

### Light Pattern (TODO)
#### Pattern 1 
#### Pattern 2 
#### Pattern 3 - Random
	
### Calculations of Imu.
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

## Members
啟萱、宣堯、修瑋
