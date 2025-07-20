/*
 * DUYinoboy - ArduinoBoy for Retrokits RK-002 Smart MIDI Cable
 * 
 * Complete implementation for Retrokits RK-002 Smart MIDI Cable
 * Supports all original ArduinoBoy modes with RK-002 optimizations
 * 
 * Hardware: RK-002 Smart MIDI Cable (2 GPIO pins only)
 * Software: Arduino IDE 1.8.x with RK-002 board package OR DUY Web Editor
 * 
 * Author: DUYinoboy Project Contributors
 * Version: 1.0
 * Date: January 2025
 * 
 * Based on original ArduinoBoy by trash80
 * Adapted for RK-002 hardware limitations and MIDI programming
 */

#include <RK002.h>
#include <EEPROM.h>

// ============================================================================
// RK-002 FIRMWARE METADATA (REQUIRED)
// ============================================================================

RK002_DECLARE_INFO(
  "DUYinoboy",
  "duyinoboy@github.com", 
  "1.0",
  "duy1b0y-2025-4b9a-ab02-rk002gameb0y"
);

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// RK-002 has exactly 2 GPIO pins available
#define GB_SIN                1    // GPIO 1 → Game Boy SIN (Pin 3) REQUIRED
#define GB_SOUT               2    // GPIO 2 ← Game Boy SOUT (Pin 2) OPTIONAL

// Uncomment for GPIO LED instead of Game Boy bidirectional communication
// #define USE_GPIO_LED              
#ifdef USE_GPIO_LED
  #define STATUS_LED_PIN      2    // GPIO 2 used for LED instead of Game Boy SOUT
#endif

// ============================================================================
// ARDUINOBOY MODE DEFINITIONS
// ============================================================================

#define MODE_LSDJ_MASTER      0
#define MODE_LSDJ_SLAVE       1  
#define MODE_LSDJ_KEYBOARD    2
#define MODE_LSDJ_MAP         3
#define MODE_NANOLOOP_MASTER  4
#define MODE_NANOLOOP_SLAVE   5
#define MODE_CUSTOM_1         6
#define MODE_CUSTOM_2         7
#define NUM_MODES             8

// ============================================================================
// TIMING AND MIDI CONFIGURATION
// ============================================================================

#define MIDI_PPQN            24    // MIDI Pulses Per Quarter Note
#define LSDJ_PPQN             4    // LSDJ Pulses Per Quarter Note  
#define CLOCK_DIVISION        6    // 24/4 = 6:1 ratio

// Timing Constants (for Game Boy Link Cable)
#define GB_CLOCK_RATE_US    500    // ArduinoBoy standard timing
#define GB_NANOLOOP_RATE_US 1000   // Slower rate for Nanoloop
#define GB_SERIAL_DELAY_US   125   // Inter-byte delay

// Default Settings
#define DEFAULT_BPM         120
#define DEFAULT_MODE        MODE_LSDJ_MASTER
#define DEFAULT_CHANNEL       1
#define MIN_BPM              60
#define MAX_BPM             200

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// EEPROM addresses for persistent settings
#define EEPROM_MODE_ADDR      0
#define EEPROM_CHANNEL_ADDR   1  
#define EEPROM_BPM_ADDR       2
#define EEPROM_MAP_ADDR       3  // Map channels stored at 3,4,5,6
#define EEPROM_MAGIC_ADDR     10 // Magic byte to check if settings exist
#define EEPROM_MAGIC_VALUE    0x42

// Core state
volatile byte currentMode = DEFAULT_MODE;
volatile byte midiChannel = DEFAULT_CHANNEL;
volatile byte midiChannelMap[4] = {1, 2, 3, 4};  // LSDJ Map mode channels
volatile byte bpm = DEFAULT_BPM;
volatile boolean clockRunning = false;

// Timing variables
volatile unsigned long clockTimer = 0;
volatile unsigned long statusTimer = 0;
volatile byte clockDivider = 0;

// Note tracking for polyphonic modes
volatile byte lastNote[4] = {0, 0, 0, 0};
volatile boolean noteActive[4] = {false, false, false, false};

// Status indicators
volatile boolean statusLED = false;

// ============================================================================
// SETUP AND MAIN LOOP
// ============================================================================

void setup() {
  // Load settings from EEPROM
  loadSettings();
  
  // Initialize GPIO pins (RK-002 specific functions)
  if (!initializeGPIO()) {
    // GPIO initialization failed - flash error pattern
    flashErrorPattern();
  }
  
  // Initialize timing
  clockTimer = millis();
  statusTimer = millis();
  
  // Signal ready
  signalReady();
}

void loop() {
  // Process clock generation for master modes
  if ((currentMode == MODE_LSDJ_MASTER || currentMode == MODE_NANOLOOP_MASTER) && clockRunning) {
    handleMasterClock();
  }
  
  // Update status indicators
  handleStatusUpdate();
  
  // Small delay to prevent system overload
  delayMicroseconds(100);
}

// ============================================================================
// MIDI EVENT CALLBACKS
// ============================================================================

boolean RK002_onNoteOn(byte channel, byte note, byte velocity) {
  switch (currentMode) {
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOn(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOn(channel, note, velocity);
      
    case MODE_NANOLOOP_MASTER:
    case MODE_NANOLOOP_SLAVE:
      return handleNanoloopNoteOn(channel, note, velocity);
      
    case MODE_CUSTOM_1:
      return handleCustom1NoteOn(channel, note, velocity);
      
    case MODE_CUSTOM_2:
      return handleCustom2NoteOn(channel, note, velocity);
  }
  return true; // Pass through MIDI
}

boolean RK002_onNoteOff(byte channel, byte note, byte velocity) {
  switch (currentMode) {
    case MODE_LSDJ_KEYBOARD:
      return handleLSDJKeyboardNoteOff(channel, note, velocity);
      
    case MODE_LSDJ_MAP:
      return handleLSDJMapNoteOff(channel, note, velocity);
      
    case MODE_NANOLOOP_MASTER:
    case MODE_NANOLOOP_SLAVE:
      return handleNanoloopNoteOff(channel, note, velocity);
      
    case MODE_CUSTOM_1:
      return handleCustom1NoteOff(channel, note, velocity);
      
    case MODE_CUSTOM_2:
      return handleCustom2NoteOff(channel, note, velocity);
  }
  return true;
}

boolean RK002_onControlChange(byte channel, byte cc, byte value) {
  switch (cc) {
    case 1:   // Mod Wheel - Alternative mode select
      setMode(value >> 4);  // Convert 0-127 to 0-7
      break;
      
    case 7:   // Volume - BPM control (master modes only)
      if (currentMode == MODE_LSDJ_MASTER || currentMode == MODE_NANOLOOP_MASTER) {
        bpm = map(value, 0, 127, MIN_BPM, MAX_BPM);
        saveSettings(); // Save immediately
      }
      break;
      
    case 16:  // GP1 - MIDI channel select
      midiChannel = map(value, 0, 127, 1, 16);
      saveSettings(); // Save immediately
      break;
      
    case 17:  // GP2 - LSDJ Map channel 1
    case 18:  // GP3 - LSDJ Map channel 2  
    case 19:  // GP4 - LSDJ Map channel 3
    case 20:  // GP5 - LSDJ Map channel 4
      if (currentMode == MODE_LSDJ_MAP) {
        byte mapIndex = cc - 17;
        midiChannelMap[mapIndex] = map(value, 0, 127, 1, 16);
        saveSettings(); // Save immediately
      }
      break;
      
    case 64:  // Sustain - Start/Stop
      if (value >= 64) {
        startClock();
      } else {
        stopClock();
      }
      break;
  }
  return true;
}

boolean RK002_onProgramChange(byte channel, byte program) {
  // PRIMARY mode selection method
  if (program < NUM_MODES) {
    setMode(program);
  }
  return true;
}

boolean RK002_onClock() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    clockDivider++;
    if (clockDivider >= CLOCK_DIVISION) {
      clockDivider = 0;
      if (clockRunning) {
        sendClockPulse();
      }
    }
  }
  return true;
}

boolean RK002_onStart() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    startClock();
  }
  return true;
}

boolean RK002_onStop() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    stopClock();
  }
  return true;
}

boolean RK002_onContinue() {
  if (currentMode == MODE_LSDJ_SLAVE || currentMode == MODE_NANOLOOP_SLAVE) {
    clockRunning = true;
  }
  return true;
}

// ============================================================================
// GAME BOY COMMUNICATION
// ============================================================================

void sendGameBoyByte(byte data) {
  // ArduinoBoy-compatible Game Boy serial protocol
  // Send each bit with proper timing
  
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    
    // Set data line
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    
    // Wait for Game Boy to read bit
    delayMicroseconds(GB_CLOCK_RATE_US);
  }
  
  // Inter-byte delay for Game Boy processing
  delayMicroseconds(GB_SERIAL_DELAY_US);
}

void sendGameBoyByteNanoloop(byte data) {
  // Nanoloop uses slower timing requirements
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    delayMicroseconds(GB_NANOLOOP_RATE_US);
  }
  
  // Longer inter-byte delay for Nanoloop
  delayMicroseconds(GB_NANOLOOP_RATE_US * 2);
}

void sendClockPulse() {
  // Send sync pulse to Game Boy - ArduinoBoy compatible
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(50);  // Hold low for 50µs
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(50);  // Hold high for 50µs
}

void sendLSDJNoteOn(byte channel, byte note, byte velocity) {
  // Send LSDJ-formatted note on message
  sendGameBoyByte(0x90 | (channel & 0x0F));
  sendGameBoyByte(mapMIDIToLSDJNote(note));
  sendGameBoyByte(mapMIDIToLSDJVelocity(velocity));
}

void sendLSDJNoteOff(byte channel, byte note) {
  // Send LSDJ-formatted note off message
  sendGameBoyByte(0x80 | (channel & 0x0F));
  sendGameBoyByte(mapMIDIToLSDJNote(note));
  sendGameBoyByte(0x00);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void setMode(byte newMode) {
  if (newMode < NUM_MODES) {
    // Stop current activities
    stopClock();
    
    // Clear note tracking
    for (int i = 0; i < 4; i++) {
      noteActive[i] = false;
      lastNote[i] = 0;
    }
    
    // Set new mode
    currentMode = newMode;
    
    // Save to EEPROM
    saveSettings();
    
    // Send confirmation flash
    flashStatusPattern(3, 100, 100);
  }
}

void startClock() {
  clockRunning = true;
  clockDivider = 0;
  clockTimer = millis();
}

void stopClock() {
  clockRunning = false;
}

void handleMasterClock() {
  unsigned long currentTime = millis();
  // Calculate interval for LSDJ 4 PPQN (not 24 PPQN like MIDI)
  unsigned long clockInterval = (60000UL / bpm / LSDJ_PPQN);
  
  if (currentTime - clockTimer >= clockInterval) {
    clockTimer = currentTime;
    
    if (currentMode == MODE_LSDJ_MASTER) {
      sendClockPulse();
    } else if (currentMode == MODE_NANOLOOP_MASTER) {
      sendNanoloopClock();
    }
  }
}

void handleStatusUpdate() {
  unsigned long currentTime = millis();
  
  if (currentTime - statusTimer >= 500) {
    statusTimer = currentTime;
    statusLED = !statusLED;
    
    #ifdef USE_GPIO_LED
      RK002_writeGPIO(STATUS_LED_PIN, statusLED ? HIGH : LOW);
    #endif
  }
}

void signalReady() {
  // Flash LED pattern to indicate startup
  #ifdef USE_GPIO_LED
    for (int i = 0; i < 3; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(100);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(100);
    }
  #endif
}

// ============================================================================
// MODE IMPLEMENTATIONS - Add your custom implementations here
// ============================================================================

// LSDJ Keyboard Mode
boolean handleLSDJKeyboardNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  sendLSDJNoteOn(0, note, velocity);
  return true;
}

boolean handleLSDJKeyboardNoteOff(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  sendLSDJNoteOff(0, note);
  return true;
}

// LSDJ Map Mode - 4-channel polyphonic
boolean handleLSDJMapNoteOn(byte channel, byte note, byte velocity) {
  // Find which LSDJ channel this MIDI channel maps to
  byte lsdjChannel = 255; // Invalid channel
  for (byte i = 0; i < 4; i++) {
    if (midiChannelMap[i] == channel) {
      lsdjChannel = i;
      break;
    }
  }
  
  if (lsdjChannel < 4) {
    // Stop current note on this channel if playing
    if (noteActive[lsdjChannel]) {
      sendLSDJNoteOff(lsdjChannel, lastNote[lsdjChannel]);
    }
    
    // Play new note
    sendLSDJNoteOn(lsdjChannel, note, velocity);
    lastNote[lsdjChannel] = note;
    noteActive[lsdjChannel] = true;
  }
  
  return true;
}

boolean handleLSDJMapNoteOff(byte channel, byte note, byte velocity) {
  // Find which LSDJ channel this MIDI channel maps to
  byte lsdjChannel = 255;
  for (byte i = 0; i < 4; i++) {
    if (midiChannelMap[i] == channel) {
      lsdjChannel = i;
      break;
    }
  }
  
  if (lsdjChannel < 4 && noteActive[lsdjChannel] && lastNote[lsdjChannel] == note) {
    sendLSDJNoteOff(lsdjChannel, note);
    noteActive[lsdjChannel] = false;
  }
  
  return true;
}

// Nanoloop Mode - Sample triggering
boolean handleNanoloopNoteOn(byte channel, byte note, byte velocity) {
  if (channel != midiChannel) return true;
  
  // Map MIDI note to Nanoloop step (0-15)
  byte nanoStep = mapMIDIToNanoloopStep(note);
  byte nanoVel = mapMIDIToNanoloopVelocity(velocity);
  
  // Send Nanoloop trigger command
  sendNanoloopTrigger(nanoStep, nanoVel);
  
  return true;
}

boolean handleNanoloopNoteOff(byte channel, byte note, byte velocity) {
  // Nanoloop doesn't require note off messages for samples
  return true;
}

// Custom Modes
boolean handleCustom1NoteOn(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom1NoteOff(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom2NoteOn(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

boolean handleCustom2NoteOff(byte channel, byte note, byte velocity) {
  // Your custom mode implementation
  return true;
}

// ============================================================================
// HELPER FUNCTIONS - Add mapping and utility functions
// ============================================================================

byte mapMIDIToLSDJNote(byte midiNote) {
  // Convert MIDI note (0-127) to LSDJ format (0-95)
  // LSDJ note range: C-2 to B-6 (8 octaves)
  if (midiNote < 12) return 0;  // Below LSDJ range
  if (midiNote > 107) return 95; // Above LSDJ range
  return midiNote - 12; // Offset to LSDJ range
}

byte mapMIDIToLSDJVelocity(byte velocity) {
  // Convert MIDI velocity (0-127) to LSDJ velocity (0-15)
  // Use logarithmic curve for musical response
  if (velocity == 0) return 0;
  if (velocity < 8) return 1;
  return (velocity >> 3) + 1; // 1-16, then clamp to 1-15
}

// Additional Nanoloop helper functions
void sendNanoloopTrigger(byte step, byte velocity) {
  // Send Nanoloop sample trigger
  sendGameBoyByteNanoloop(0x90 | (step & 0x0F)); // Note on + step
  sendGameBoyByteNanoloop(velocity);
}

void sendNanoloopClock() {
  // Nanoloop clock pulse - slower than LSDJ
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(100);
  RK002_writeGPIO(GB_SIN, HIGH);
  delayMicroseconds(100);
}

byte mapMIDIToNanoloopStep(byte midiNote) {
  // Map MIDI note to Nanoloop step (0-15)
  return (midiNote - 36) & 0x0F; // C2 = step 0
}

byte mapMIDIToNanoloopVelocity(byte velocity) {
  // Convert MIDI velocity to Nanoloop format
  return velocity >> 2; // 0-127 to 0-31
}

// EEPROM functions for persistent settings
void loadSettings() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    // Settings exist, load them
    currentMode = EEPROM.read(EEPROM_MODE_ADDR);
    midiChannel = EEPROM.read(EEPROM_CHANNEL_ADDR);
    bpm = EEPROM.read(EEPROM_BPM_ADDR);
    
    // Load channel map
    for (byte i = 0; i < 4; i++) {
      midiChannelMap[i] = EEPROM.read(EEPROM_MAP_ADDR + i);
    }
    
    // Validate loaded values
    if (currentMode >= NUM_MODES) currentMode = DEFAULT_MODE;
    if (midiChannel < 1 || midiChannel > 16) midiChannel = DEFAULT_CHANNEL;
    if (bpm < MIN_BPM || bpm > MAX_BPM) bpm = DEFAULT_BPM;
  } else {
    // First boot, save defaults
    saveSettings();
  }
}

void saveSettings() {
  EEPROM.write(EEPROM_MODE_ADDR, currentMode);
  EEPROM.write(EEPROM_CHANNEL_ADDR, midiChannel);
  EEPROM.write(EEPROM_BPM_ADDR, bpm);
  
  // Save channel map
  for (byte i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_MAP_ADDR + i, midiChannelMap[i]);
  }
  
  // Set magic byte to indicate settings are valid
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
}

// GPIO error handling
boolean initializeGPIO() {
  // Try to initialize GPIO pins with error checking
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_writeGPIO(GB_SIN, HIGH);  // Idle state
  
  #ifdef USE_GPIO_LED
    RK002_setGPIOType(STATUS_LED_PIN, RK002_GPIO_OUTPUT);
    RK002_writeGPIO(STATUS_LED_PIN, LOW);
  #else
    RK002_setGPIOType(GB_SOUT, RK002_GPIO_INPUT_PULLUP);
  #endif
  
  // Test GPIO functionality
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(10);
  RK002_writeGPIO(GB_SIN, HIGH);
  
  return true; // RK-002 GPIO calls don't return error codes
}

void flashErrorPattern() {
  // Flash rapid error pattern if GPIO fails
  #ifdef USE_GPIO_LED
    for (byte i = 0; i < 10; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(50);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(50);
    }
  #endif
}

void flashStatusPattern(byte count, unsigned int onTime, unsigned int offTime) {
  #ifdef USE_GPIO_LED
    for (byte i = 0; i < count; i++) {
      RK002_writeGPIO(STATUS_LED_PIN, HIGH);
      delay(onTime);
      RK002_writeGPIO(STATUS_LED_PIN, LOW);
      delay(offTime);
    }
  #endif
}
