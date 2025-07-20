/*
 * Basic LSDJ Clock Sync Example
 * 
 * LSDJ MIDI clock follower for RK-002
 * Demonstrates basic Game Boy communication (slave sync only)
 */

#include <RK002.h>

RK002_DECLARE_INFO(
  "LSDJ Clock Sync",
  "duyinoboy@github.com", 
  "1.0",
  "lsdj-sync-basic-2025-example-uuid"
);

// RK-002 is MIDI input only - LSDJ follows external clock
#define MODE_LSDJ_SYNC 0  // LSDJ MIDI Clock Sync (slave mode only)

// GPIO pins
#define GB_SIN      1
#define STATUS_LED  2

// Timing
#define CLOCK_DIVISION 6  // 24 PPQN to 4 PPQN

// State variables
byte currentMode = MODE_LSDJ_SYNC;
boolean clockRunning = false;
byte clockCounter = 0;
unsigned long lastClockTime = 0;

void setup() {
  // Initialize GPIO
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_setGPIOType(STATUS_LED, RK002_GPIO_OUTPUT);
  
  // Set default state
  RK002_writeGPIO(GB_SIN, HIGH);
  flashLED(3); // Ready signal
}

void loop() {
  // Handle master clock generation
  if (currentMode == MODE_MASTER && clockRunning) {
    generateMasterClock();
  }
}

// MIDI event handlers
boolean RK002_onProgramChange(byte channel, byte program) {
  if (program <= 1) {
    currentMode = program;
    flashLED(currentMode + 1); // Visual feedback
  }
  return true;
}

boolean RK002_onControlChange(byte channel, byte cc, byte value) {
  if (cc == 64) { // Sustain pedal
    if (value >= 64) {
      startClock();
    } else {
      stopClock();
    }
  }
  return true;
}

boolean RK002_onClock() {
  if (currentMode == MODE_SLAVE) {
    clockCounter++;
    if (clockCounter >= CLOCK_DIVISION) {
      clockCounter = 0;
      if (clockRunning) {
        sendGameBoyClock();
      }
    }
  }
  return true;
}

boolean RK002_onStart() {
  if (currentMode == MODE_SLAVE) {
    startClock();
  }
  return true;
}

boolean RK002_onStop() {
  if (currentMode == MODE_SLAVE) {
    stopClock();
  }
  return true;
}

// Clock functions
void startClock() {
  clockRunning = true;
  clockCounter = 0;
  lastClockTime = millis();
  RK002_writeGPIO(STATUS_LED, HIGH);
}

void stopClock() {
  clockRunning = false;
  RK002_writeGPIO(STATUS_LED, LOW);
}

void generateMasterClock() {
  unsigned long currentTime = millis();
  unsigned long interval = 125; // 120 BPM = 125ms per 4 PPQN tick
  
  if (currentTime - lastClockTime >= interval) {
    lastClockTime = currentTime;
    sendGameBoyClock();
  }
}

void sendGameBoyClock() {
  // Send sync pulse to Game Boy
  RK002_writeGPIO(GB_SIN, LOW);
  delayMicroseconds(10);
  RK002_writeGPIO(GB_SIN, HIGH);
}

void flashLED(byte count) {
  for (byte i = 0; i < count; i++) {
    RK002_writeGPIO(STATUS_LED, HIGH);
    delay(100);
    RK002_writeGPIO(STATUS_LED, LOW);
    delay(100);
  }
}
