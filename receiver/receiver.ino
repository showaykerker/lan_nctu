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
uint8_t current_program = 0;
uint8_t current_2w = 0;
uint8_t current_acc = 0;
uint16_t current_BPM = 300;

midiEventPacket_t rx;

void loop() {

	do {
		rx = MidiUSB.read();
		if (rx.header != 0) {
			/*
			Serial.print("Received: ");
			Serial.print(rx.byte1); Serial.print(',');
			Serial.print(rx.byte2); Serial.print(',');
			Serial.println(rx.byte3);
			*/
			uint16_t cmd_BPM = 60.116 * exp(0.0545 * min(max(rx.byte3, 0), 40));
			
			if (cmd_BPM != current_BPM){
				//Serial.print("Write: "); Serial.println(cmd_BPM);
				Serial1.print(int(cmd_BPM)); Serial1.print(',');
				current_BPM = cmd_BPM;
			}
		}
	} while (rx.header != 0);

	
	if (Serial1.available()) {
		val = Serial1.read();

		if (val==','){ 
			bool program_has_change = false ;
			bool btn_has_change = false ;
			bool sw_has_change = false ;
			bool acc_has_change = false ;
			
			uint16_t cmd_int = cmd.toInt();
			uint8_t cmd_val = cmd_int;
			uint8_t cmd_2w = (cmd_val >> 3) % 4;
			uint8_t cmd_program = (cmd_val >> 5) % 4;
			uint8_t cmd_acc = cmd_int >> 8;
					
			//Serial.println(cmd_val);
				
			if (cmd_program != current_program){
				controlChange(0, current_program, 0);
				controlChange(0, cmd_program, 127);
				current_program = cmd_program;
				program_has_change = true;
			}
			
			for(int i = 0;i<=2;i++){ // Reading Buttons
				bool btn_n_on = cmd_val % 2;
				if (btn_n_on != current_btn[i]){
					current_btn[i] = btn_n_on;
					btn_has_change = true;
				}
				if (btn_has_change){
					uint8_t btn_val = val2note[btn_array_to_value(current_btn)];
					if (t_last_note != 0)
						noteOff(0, previous_btn_val, VELOCITY);
					noteOn(0, btn_val, VELOCITY);
					t_last_note = millis();
					previous_btn_val = btn_val;
				}
				cmd_val = cmd_val >> 1;
			}
			if (millis()-t_last_note>=NOTE_LAST && t_last_note!=0){
				noteOff(0, previous_btn_val, VELOCITY);
				t_last_note = 0;
				btn_has_change = true ;
			}
			//Serial.println(t_last_note);
			
			if (cmd_2w != current_2w){ // Reading 2-way switch
				//uint8_t sw = cmd_val % 4; // 0 1 2
				controlChange(0, current_2w+12, 0);
				controlChange(0, cmd_2w+12, 127);
				current_2w = cmd_2w;
				sw_has_change = true ;
			}		
			
			if (current_acc != cmd_acc){ // Reading Acceleration
				uint8_t output_val = 127 - int(float(cmd_acc) * 80. / 127.);
				controlChange(0, 4, output_val);
				//Serial.print(cmd_acc);
				//Serial.print(", ");
				//Serial.println(output_val);
				current_acc = cmd_acc;
				acc_has_change = true ;
			}
			
			if (program_has_change || btn_has_change || sw_has_change || acc_has_change) MidiUSB.flush();
			/*
			Serial.print(uint8_t(cmd_int));
			Serial.print(" ");
			Serial.println(cmd_acc);
			*/
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
