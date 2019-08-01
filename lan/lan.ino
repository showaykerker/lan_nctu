#include <Adafruit_NeoPixel.h>
#define pinStripTop 5
#define pinStripBot 4
#define nStripPixel 24
Adafruit_NeoPixel strip_top(nStripPixel, pinStripTop, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_bot(nStripPixel, pinStripBot, NEO_GRB + NEO_KHZ800);
uint8_t curr_rgbB[24][4] = {0}; // r, g, b, Brightness
uint8_t current_light_set = 0; // 0, 1, 2, 3
uint8_t light_set = 0;
uint32_t current_light_count = 0;
float light_count_length[4] = {1000, 480, 90, 100}; // ms
float light_count_cut[4]    = {1000, 160, 30, 100}; // ms
uint16_t pattern2_last_move = 0;
#define LPF_RAND_STRIP 0.95

#include "imu.hpp"
#define pinBTN1 6  // 0
#define pinBTN2 8  // 1
#define pinBTN3 9  // 2
#define pin2WB A0  // 3, 4
#define pinSW A1   // 5, 6

#define SAMPLE_TIME 10 // ms
#define LPF_DELAY_PREV 0.99
#define DELAY_MIN 2
#define DELAY_MAX 200

imu = lanLSM9DS1();

uint8_t read_btns(void);
unsigned long int t = 10, last_t = 0;
float delay_filtered = 0.0;
bool to_print = true ;
uint8_t to_read = 0;

uint16_t min_ = 1000, max_ = 0;

void change_light(uint8_t val, uint16_t val_delay){
	uint8_t brightness = uint8_t(55 + int((255 - val_delay) * 200 / 255));
	if (light_set != current_light_set){
		current_light_count = 0;
		current_light_set = light_set;
	}
	
	for (uint8_t i_top = 0; i_top<24; i_top++){
		uint8_t i_bot = 23 - i_top;
		if (light_set == 1){ // rgb take turn
			if (0 <= current_light_count && current_light_count < light_count_cut[1]){
				curr_rgbB[i_top][0] = uint8_t(float(light_count_cut[1] - current_light_count)/light_count_cut[1]*255.0);
				curr_rgbB[i_top][1] = uint8_t(float(current_light_count)/light_count_cut[1]*255.0);
			}
			else if(light_count_cut[1] <= current_light_count && current_light_count < light_count_cut[1]*2){
				curr_rgbB[i_top][1] = uint8_t(float(light_count_cut[1]*2 - current_light_count)/light_count_cut[1]*255.0);
				curr_rgbB[i_top][2] = uint8_t(float(current_light_count - light_count_cut[1])/light_count_cut[1]*255.0);
			}
			else if (light_count_cut[1]*2 <= current_light_count && current_light_count < light_count_cut[1]*3){
				curr_rgbB[i_top][2] = uint8_t(float(light_count_cut[1]*3 - current_light_count)/light_count_cut[1]*255.0);
				curr_rgbB[i_top][0] = uint8_t(float(current_light_count - light_count_cut[1]*2)/light_count_cut[1]*255.0);
			}
			curr_rgbB[i_top][3] = brightness;
		}
		else if (light_set == 2){ // rgb circle
			if (current_light_count == 0 || current_light_count == uint8_t(light_count_cut[light_set])-1 || current_light_count == uint8_t(light_count_cut[light_set])*2-1){
				for (int j =0; j<=23; j++){
					for (int k = 0; k<=2; k++) curr_rgbB[j][k] = 0;
				}
			}
			if (current_light_count == 0){
				curr_rgbB[0][1] = 255;
				pattern2_last_move = 0;
			}
			else if (current_light_count == uint8_t(light_count_cut[light_set])-1){
				curr_rgbB[0][2] = 255;
				pattern2_last_move = current_light_count;
			}
			else if (current_light_count == uint8_t(light_count_cut[light_set])*2-1){
				curr_rgbB[0][0] = 255;
				pattern2_last_move = current_light_count;
			}
			else{
				// 60~255
				float slice = (255.-60.)/16.;
				float move_every = light_count_cut[2] / 24.;
				if (current_light_count-pattern2_last_move>move_every){
					if (i_top == 0){ 
						for(int j = 0; j<=2; j++){
							if ( float(curr_rgbB[i_top][j]) > 60) curr_rgbB[i_top][j] -= uint8_t(slice);
							else curr_rgbB[i_top][j] = 0;
						}
					}
					else{
						for(int j = 0; j<=2; j++) curr_rgbB[24-i_top][j] = curr_rgbB[24-i_top-1][j];
						if (i_top == 23) pattern2_last_move += move_every;
					}
					
				}
			}
		}
		
		else if (light_set == 3){ // random
			uint8_t r = uint8_t(random(100));
			uint8_t vr, vg, vb, vB = 0;
			if (r > 99){
				vr = uint8_t( random(211) + 45 );
				vg = uint8_t( random(211) + 45 );
				vb = uint8_t( random(211) + 45 );
				vB = uint8_t( random(100) + 45 );
			}
			else if (r > 97){
				vr = uint8_t( random(111) + 45 );
				vg = uint8_t( random(111) + 45 );
				vb = uint8_t( random(111) + 45 );
				vB = uint8_t( random(55) + 45 );
			}
			else if (r > 95){
				vr = uint8_t( random(81) + 45 );
				vg = uint8_t( random(61) + 45 );
				vb = uint8_t( random(61) + 45 );
				vB = uint8_t( random(75) + 45 );
			}
			else if (r > 80){
				vr =  vg = vb = vB = 0;
			}
			curr_rgbB[i_top][0] = uint8_t(LPF_RAND_STRIP * curr_rgbB[i_top][0] + (1 - LPF_RAND_STRIP) * vr);
			curr_rgbB[i_top][1] = uint8_t(LPF_RAND_STRIP * curr_rgbB[i_top][1] + (1 - LPF_RAND_STRIP) * vg);
			curr_rgbB[i_top][2] = uint8_t(LPF_RAND_STRIP * curr_rgbB[i_top][2] + (1 - LPF_RAND_STRIP) * vb);
			curr_rgbB[i_top][3] = uint8_t(LPF_RAND_STRIP * curr_rgbB[i_top][3] + (1 - LPF_RAND_STRIP) * vB);
			if (r % 25 == 0){
				curr_rgbB[i_top][random(4)] += uint8_t(random(70));
			}
		}
		
		else{
			curr_rgbB[i_top][0] = curr_rgbB[i_top][1] = curr_rgbB[i_top][2] = curr_rgbB[i_top][3] = 0;
		}
		
		strip_top.setPixelColor(i_top, curr_rgbB[i_top][0], curr_rgbB[i_top][1], curr_rgbB[i_top][2]);
		strip_bot.setPixelColor(i_bot, curr_rgbB[i_top][0], curr_rgbB[i_top][1], curr_rgbB[i_top][2]);
		
	}
	//Serial.println(current_light_count);
	current_light_count = (current_light_count+1) % int(light_count_length[current_light_set]);
	//strip_top.setBrightness(curr_rgbB[0][3]);
	//strip_bot.setBrightness(curr_rgbB[0][3]);
	strip_top.show();
	strip_bot.show();
	
}

void setup() {
	
	Serial.begin(9600);
	Serial1.begin(38400);
	strip_top.begin();
	strip_top.show();
	strip_bot.begin();
	strip_bot.show();
	
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
	
	uint16_t val_delay = map(delay_filtered, DELAY_MIN, DELAY_MAX, 0, 127);
	uint16_t final_val = val_delay * 256 + val;
	
	change_light(val, val_delay);
	
	//Serial.println(t);
	// Sampling rate is 100hz, update rate is 50hz.
	to_read = (to_read+1)%8;
	if (to_read == 0) read_Serial1();
	if (to_print){
		to_print = false ;
		Serial1.print(final_val);
		Serial1.print(',');
	}
	else to_print = true;
	
}


String cmd_BPM = "";

void read_Serial1(void){
	uint16_t bits = Serial1.available();
	if (bits == 0) return;
	else{		
		String cmd_last = "";
		for(int i = 0; i<bits; i++){
			char new_str = Serial1.read();
			if (new_str == ','){ // New BPM
				cmd_last = cmd_BPM;
				//Serial.print(cmd_BPM); Serial.print(" ");
				//Serial.println(cmd_last);
				cmd_BPM = "";
			}
			else{
				cmd_BPM += new_str;
			}
			if (i == bits-1){
				uint16_t new_BPM = cmd_last.toInt();
				
				//Serial.println(new_BPM);
				light_count_length[2] = 24000./float(new_BPM)/3.*4.;
				light_count_cut[2] = light_count_length[2]/3.;
				
				light_count_cut[1] = 16875.0 / float(new_BPM);
				light_count_length[1] = light_count_cut[1] * 3.;
				current_light_count = 0;
			}
			
		}
		
	}
}

uint8_t read_btns(void){
	uint8_t val = 0;
	if (digitalRead(pinBTN1) == HIGH) val += 1;
	if (digitalRead(pinBTN2) == HIGH) val += 2;
	if (digitalRead(pinBTN3) == HIGH) val += 4;
	
	// 0 - 683 - 1023
	int val_2wb = analogRead(pin2WB);
	//Serial.println(val_2wb);
	if (val_2wb > 800) val += 16;
	else if (val_2wb > 500) val += 8;

	// 0 - 339 - 683 - 1023
	int val_sw = analogRead(pinSW);
	//Serial.println(val_sw);
	if (val_sw > 800){
		val += 96;
		light_set = 3;
	}
	else if (val_sw > 380){
		val += 64;
		light_set = 2;
	}
	else if (val_sw > 100){
		val += 32;
		light_set = 1;
	}
	else{
		light_set = 0;
	}

	return val;
}
