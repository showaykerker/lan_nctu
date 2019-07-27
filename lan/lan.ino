#include "imu.hpp"

#define pinLEDB 4  
#define pinLEDT 5   
#define pinBTN1 6  // 0
#define pinBTN2 8  // 1
#define pinBTN3 9  // 2
#define pin2WB A0  // 3, 4
#define pinSW A1   // 5, 6

#define SAMPLE_TIME 10 // ms
#define LPF_DELAY_PREV 0.8
#define DELAY_MIN 2
#define DELAY_MAX 200

imu = lanLSM9DS1();

uint8_t read_btns(void);
unsigned long int t = 10, last_t = 0;
float delay_filtered = 0.0;
bool to_print = true ;

uint16_t min_ = 1000, max_ = 0;

void setup() {
	
	Serial.begin(9600);
	Serial1.begin(38400);
	while(!imu.is_begin());
	Serial.println("imu begins.");
}

void loop() {
	
	while(t - last_t < SAMPLE_TIME){
		t = millis();
	}
	
	last_t = t;
	
	uint8_t val = read_btns();
	uint8_t imu_status = imu.update();
	
	delay_filtered = delay_filtered * LPF_DELAY_PREV + imu.get_delay_time() * (1-LPF_DELAY_PREV);
	
	if (delay_filtered<DELAY_MIN) delay_filtered = DELAY_MIN;
	if (delay_filtered>DELAY_MAX) delay_filtered = DELAY_MAX;
	
	uint16_t val_delay = map(delay_filtered, DELAY_MIN, DELAY_MAX, 0, 255);
	uint16_t final_val = val_delay * 256 + val;
	
	// Sampling rate is 100hz, update rate is 50hz.
	if (to_print){
		to_print = false ;
		Serial.print(final_val);
		Serial.print(',');
		
	}
	else to_print = true;
	
	//Serial.print(t); Serial.print("  ");
	//Serial.print(d_acc_yz_filtered); Serial.print("  ");
	//Serial.println(delay_time_so_far);
	/*
	Serial1.print("S,");
	Serial1.print(t);
	Serial1.print(',');
	Serial1.print(val);
	Serial1.println(",E");
	*/
}

uint8_t read_btns(void){
	uint8_t val = 0;
	if (digitalRead(pinBTN1) == HIGH) val += 1;
	if (digitalRead(pinBTN2) == HIGH) val += 2;
	if (digitalRead(pinBTN3) == HIGH) val += 4;
	// 339 - 683 - 1023
	int val_2wb = analogRead(pin2WB);
	//Serial.println(val_2wb);
	if (val_2wb > 800) val += 8;
	else if (val_2wb > 500) val += 16;

	int val_sw = analogRead(pinSW);
	//Serial.println(val_sw);
	if (val_sw > 800) val += 96;
	else if (val_sw > 500) val += 64;
	else if (val_sw > 100) val += 32;

	return val;
}
