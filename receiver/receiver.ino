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
 
void loop() {
    
  if (Serial1.available()) {
    val = Serial1.read();
    //Serial.print(val);
	
    if (val == 'E'){
		//Serial.println('E');
		copy_();		
		now_rec.clean();
		Serial.println(last_rec.val);
    }
    else if(val=='\n'){} 
	else if(val==','){
		if (cmd!=""){ // ignore first , after 'S'
			//Serial.println(cmd)
			switch (now_rec.curr){
				case 1:
					now_rec.t = cmd.toInt();
					break;
				case 2:
					now_rec.val = cmd.toInt();
					break;
				case 3:
					now_rec.acc[0] = cmd.toFloat();
					break;
				case 4:
					now_rec.acc[1] = cmd.toFloat();
					break;
				case 5:
					now_rec.acc[2] = cmd.toFloat();
					break;
				case 6:
					now_rec.acc[3] = cmd.toFloat();
					break;
				case 7:
					now_rec.acc[4] = cmd.toFloat();
					break;
				case 8:
					now_rec.acc[5] = cmd.toFloat();
					break;
			}
			cmd = "";
			now_rec.curr += 1;
		}
	}
    else{
		cmd += val;
	}
	
  }
  
}
