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

void programChange(byte channel, byte control){
	midiEventPacket_t event = {0x0C, 0xC0 | channel, control, B0};
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
uint8_t current_program = 0;

void loop() {
    
  if (Serial1.available()) {
    val = Serial1.read();
	
	if (val==','){ 
		uint16_t cmd_int = cmd.toInt();
		uint8_t cmd_val = cmd_int;
		uint8_t cmd_acc = cmd_int >> 8;
		uint8_t program = cmd_val >> 5;
		if (program != current_program){
			controlChange(0, current_program, 0);
			MidiUSB.flush();
			controlChange(0, program, 125);
			MidiUSB.flush();
			controlChange(0, program, 126);
			MidiUSB.flush();
			controlChange(0, program, 127);
			MidiUSB.flush();
			current_program = program;
			
		}
		Serial.print(millis());
		Serial.print(" ");
		Serial.print(cmd_val);
		Serial.print(" ");
		Serial.print(cmd_acc);
		Serial.print(" ");
		Serial.println(program);
		cmd = "";
	}
	else cmd+=val;
	
  }
  
}
