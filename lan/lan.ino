#include "imu.hpp"

#define pinLEDB 4  
#define pinLEDT 5   
#define pinBTN1 6  // 0
#define pinBTN2 8  // 1
#define pinBTN3 9  // 2
#define pin2WB A0  // 3
#define pinSW A1   // 4

imu = lanLSM9DS1();

uint8_t read_btns(void);
unsigned long long int t, last_t = 0;

void setup() {
	Serial.begin(9600);
	Serial1.begin(38400);
	while(!imu.is_begin());
	Serial.println("imu begins.");
}

void loop() {

	uint8_t val = read_btns();
	uint8_t imu_status = imu.update();
	t = millis();
	
	while(t - last_t < 12){
		t = millis();
	}
	last_t = t;
	Serial1.print("S,");
	Serial1.print(millis());
	Serial1.print(',');
	Serial1.print(val);
	Serial1.print(',');
	Serial1.print(imu.acc[0], 2);
	Serial1.print(',');
	Serial1.print(imu.acc[1], 2);
	Serial1.print(',');
	Serial1.print(imu.acc[2], 2);
	Serial1.print(',');
	Serial1.print(imu.gyr[0], 2);
	Serial1.print(',');
	Serial1.print(imu.gyr[1], 2);
	Serial1.print(',');
	Serial1.print(imu.gyr[2], 2);
	Serial1.println(",E");
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
