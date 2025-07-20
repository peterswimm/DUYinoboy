/*
 * MIDI Note Mapper Example
 * 
 * Maps MIDI channels to Game Boy channels
 * Demonstrates polyphonic LSDJ control
 */

#include <RK002.h>

RK002_DECLARE_INFO(
  "MIDI Note Mapper",
  "duyinoboy@github.com", 
  "1.0",
  "note-mapper-2025-example-uuid"
);

// GPIO pins
#define GB_SIN  1
#define GB_SOUT 2

// Channel mapping
byte midiChannelMap[4] = {1, 2, 3, 4}; // MIDI channels 1-4 → GB channels 0-3

// Note tracking
struct GameBoyNote {
  byte midiNote;
  boolean active;
};

GameBoyNote gbNotes[4] = {{0, false}, {0, false}, {0, false}, {0, false}};

void setup() {
  // Initialize GPIO
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_setGPIOType(GB_SOUT, RK002_GPIO_INPUT_PULLUP);
  
  // Set idle state
  RK002_writeGPIO(GB_SIN, HIGH);
}

void loop() {
  // Main loop - minimal processing
}

// MIDI event handlers
boolean RK002_onNoteOn(byte channel, byte note, byte velocity) {
  // Find which Game Boy channel this MIDI channel maps to
  byte gbChannel = findGameBoyChannel(channel);
  
  if (gbChannel < 4) {
    // Send note to Game Boy channel
    sendLSDJNoteOn(gbChannel, note, velocity);
    
    // Track the note
    gbNotes[gbChannel].midiNote = note;
    gbNotes[gbChannel].active = true;
  }
  
  return true; // Pass through MIDI
}

boolean RK002_onNoteOff(byte channel, byte note, byte velocity) {
  // Find which Game Boy channel this MIDI channel maps to
  byte gbChannel = findGameBoyChannel(channel);
  
  if (gbChannel < 4 && gbNotes[gbChannel].active && gbNotes[gbChannel].midiNote == note) {
    // Send note off to Game Boy
    sendLSDJNoteOff(gbChannel, note);
    
    // Clear note tracking
    gbNotes[gbChannel].active = false;
    gbNotes[gbChannel].midiNote = 0;
  }
  
  return true;
}

boolean RK002_onControlChange(byte channel, byte cc, byte value) {
  // CC17-20: Remap MIDI channels for GB channels 1-4
  if (cc >= 17 && cc <= 20) {
    byte gbChannelIndex = cc - 17;
    byte newMidiChannel = map(value, 0, 127, 1, 16);
    midiChannelMap[gbChannelIndex] = newMidiChannel;
  }
  
  return true;
}

// Helper functions
byte findGameBoyChannel(byte midiChannel) {
  // Find which Game Boy channel corresponds to this MIDI channel
  for (byte i = 0; i < 4; i++) {
    if (midiChannelMap[i] == midiChannel) {
      return i;
    }
  }
  return 255; // Not found
}

void sendLSDJNoteOn(byte gbChannel, byte note, byte velocity) {
  // Send LSDJ note on message
  sendGameBoyByte(0x90 | (gbChannel & 0x0F)); // Note on + channel
  sendGameBoyByte(note);                      // MIDI note
  sendGameBoyByte(velocity >> 3);             // Convert to 0-15 range
}

void sendLSDJNoteOff(byte gbChannel, byte note) {
  // Send LSDJ note off message
  sendGameBoyByte(0x80 | (gbChannel & 0x0F)); // Note off + channel
  sendGameBoyByte(note);                      // MIDI note
  sendGameBoyByte(0x00);                      // Velocity 0
}

void sendGameBoyByte(byte data) {
  // Software serial implementation for Game Boy
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    
    // Set data line
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    delayMicroseconds(31); // Half bit time (~8kHz)
    
    // Clock pulse simulation
    RK002_writeGPIO(GB_SIN, LOW);
    delayMicroseconds(2);
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    
    delayMicroseconds(31); // Complete bit
  }
  
  // Return to idle
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(62);
}
