#include "received_data.hpp"
#include "MIDIUSB.h"
#include "midi_func.cpp"

#define CONTROL_TYPE 1 // 
#define NOTE_LAST 200 // ms
#define VELOCITY 127

// For control type 1 64
// 64 middle C, 65 middle C# and so on
uint8_t val2note[8] = {76, 68, 73, 66, 75, 70, 72, 64};

void setup() {
  Serial.begin(115200);
  Serial.println("BT is ready!");
  Serial1.begin(38400);
}
 
rec_data last_rec, now_rec;
char val;
String cmd = "";
bool new_data = true ;
bool current_btn[3] = {0};
uint32_t t_last_note = 0;
uint8_t previous_btn_val = 0;
uint8_t current_2w = 0;
uint8_t current_acc = 0;

void loop() {
    
  if (Serial1.available()) {
    val = Serial1.read();
	
	if (val==','){ 
		bool btn_has_change = false ;
		bool sw_has_change = false ;
		bool acc_has_change = false ;
		uint16_t cmd_int = cmd.toInt();
		uint8_t cmd_val = cmd_int;
		uint8_t cmd_acc = cmd_int >> 8;
		
		for(int i = 0;i<=2;i++){ // Reading Buttons
			bool btn = cmd_val%2;
			if (btn!=current_btn[i]){
				if (CONTROL_TYPE==0) controlChange(0, 12+i, int(btn)*127);
				current_btn[i] = btn;
				btn_has_change = true ;
			}
			if (CONTROL_TYPE==1 && btn_has_change){
				uint8_t btn_val = val2note[btn_array_to_value(current_btn)];
				if (t_last_note != 0)
					noteOff(0, previous_btn_val, VELOCITY);
				noteOn(0, btn_val, VELOCITY);
				t_last_note = millis();
				previous_btn_val = btn_val;
			}
			cmd_val = cmd_val >> 1;
		}
		if (CONTROL_TYPE == 1 && millis()-t_last_note>=NOTE_LAST && t_last_note!=0){
			noteOff(0, previous_btn_val, VELOCITY);
			t_last_note = 0;
			btn_has_change = true ;
		}
		Serial.println(t_last_note);
		
		if (cmd_val%4 != current_2w){ // Reading 2-way switch
			uint8_t sw = cmd_val%4; // 0 1 2
			controlChange(0, current_2w, 0);
			controlChange(0, sw, 127);
			current_2w = sw;
			sw_has_change = true ;
		}		
		
		if (current_acc != cmd_acc){ // Reading Acceleration.
			controlChange(0, 4, 64-int(cmd_acc/2)+10);
			current_acc = cmd_acc;
			acc_has_change = true ;
		}
		
		if (btn_has_change || sw_has_change || acc_has_change) MidiUSB.flush();
		
		Serial.print(uint8_t(cmd_int));
		Serial.print(" ");
		Serial.println(cmd_acc);
	
		cmd = "";
	}
	else cmd+=val;
	
  }
}

void copy_(void){
	last_rec.t = now_rec.t;
	last_rec.val = now_rec.val;
	for(int i = 0;i<=2;i++){
		last_rec.acc[i] = now_rec.acc[i];
		last_rec.gyr[i] = now_rec.gyr[i];
	}
	last_rec.curr = now_rec.curr;
}
