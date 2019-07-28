#include "received_data.hpp"
#include "MIDIUSB.h"

void noteOn(byte channel, byte pitch, byte velocity) {
	midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
	MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
	midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
	MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
	midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
	MidiUSB.sendMIDI(event);
}

rec_data last_rec, now_rec;

char val;
String cmd = "";

void setup() {
  Serial.begin(115200);
  Serial.println("BT is ready!");

  Serial1.begin(38400);
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

bool new_data = true ;

bool current_btn[3] = {0};
uint8_t current_2w = 0;
uint8_t current_program = 0;
uint8_t current_acc = 0;

void loop() {
    
  if (Serial1.available()) {
    val = Serial1.read();
	
	if (val==','){ 
		bool has_change = false ;
		uint16_t cmd_int = cmd.toInt();
		uint8_t cmd_val = cmd_int;
		uint8_t cmd_acc = cmd_int >> 8;
		uint8_t cmd_program = cmd_val >> 5;
		
		if (cmd_program != current_program){ // Reading top switch ( program )
			controlChange(0, current_program, 0);
			controlChange(0, cmd_program, 127);
			current_program = cmd_program;
			has_change = true ;
		}
		
		for(int i = 0;i<=2;i++){ // Reading Buttons
			bool btn = cmd_val%2;
			if (btn!=current_btn[i]){
				controlChange(0, 5+i, int(btn)*127);
				current_btn[i] = btn;
				has_change = true ;
			}
			cmd_val = cmd_val >> 1;
			
		}
		
		if (cmd_val%4 != current_2w){ // Reading 2-way switch
			uint8_t sw = cmd_val%4;
			if (sw == 0) sw = 0;
			else if(sw == 1) sw = 63;
			else if(sw == 2) sw = 127;
			controlChange(0, 8, sw);
			current_2w = cmd_val%4;
			has_change = true ;
		}		
		
		if (current_acc != cmd_acc){ // Reading Acceleration.
			controlChange(0, 4, 64-int(cmd_acc/2)+10);
			current_acc = cmd_acc;
			has_change = true ;
		}
		
		if (has_change) MidiUSB.flush();
		
		Serial.print(uint8_t(cmd_int));
		Serial.print(" ");
		Serial.println(cmd_acc);
	
		cmd = "";
	}
	else cmd+=val;
	
  }
  
}
