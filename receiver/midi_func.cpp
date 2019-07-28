#ifndef MIDI_FUNC_CPP
#define MIDI_FUNC_CPP

#include "MIDIUSB.h"
#include <Arduino.h>

inline uint8_t btn_array_to_value(bool *arr){
	uint8_t val = 0;
	for (int i=2; i>=0; i--){
		val += uint8_t(arr[i]);
		val = val << 1;
	}
	return val >> 1;
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

inline void noteOn(byte channel, byte pitch, byte velocity) {
  Serial.print('On'); Serial.print(", ");
  Serial.print(channel); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.println(velocity); 
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

inline void noteOff(byte channel, byte pitch, byte velocity) {
  Serial.print('Off'); Serial.print(", ");
  Serial.print(channel); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.println(velocity); 
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

inline void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

#endif