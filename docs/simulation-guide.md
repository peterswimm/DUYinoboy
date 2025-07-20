# DUYinoboy Simulation Guide

Complete guide for testing and simulating DUYinoboy code using online Arduino development resources.

## 🌐 Online Arduino Simulators

### 1. [Wokwi Arduino Simulator](https://wokwi.com/arduino) (Recommended)
**URL**: https://wokwi.com/arduino

**Features**:
- Real-time Arduino simulation
- MIDI input support
- GPIO pin visualization
- Serial monitor
- Oscilloscope for timing analysis
- **Note**: RK-002 is MIDI input only (no MIDI output)

**Setup for DUYinoboy**:
```cpp
// Wokwi-compatible version (without RK002.h)
#define RK002_GPIO_OUTPUT 1
#define RK002_GPIO_INPUT_PULLUP 2

void RK002_setGPIOType(int pin, int mode) {
  pinMode(pin, mode == RK002_GPIO_OUTPUT ? OUTPUT : INPUT_PULLUP);
}

void RK002_writeGPIO(int pin, int value) {
  digitalWrite(pin, value);
}

int RK002_readGPIO(int pin) {
  return digitalRead(pin);
}
```

### 2. [Tinkercad Circuits](https://www.tinkercad.com/circuits)
**URL**: https://www.tinkercad.com/circuits

**Features**:
- Visual circuit builder
- Arduino Uno simulation
- Real-time component interaction
- Breadboard layout

**Limitations**:
- No native MIDI support
- Limited to basic GPIO simulation

### 3. [SimulIDE](https://simulide.com/)
**URL**: https://simulide.com/

**Features**:
- Advanced electronic simulation
- Multiple microcontroller support
- Logic analyzer
- Oscilloscope

### 4. [Arduino Web Editor](https://create.arduino.cc/editor)
**URL**: https://create.arduino.cc/editor

**Features**:
- Official Arduino web editor
- Cloud compilation
- Library management
- Code syntax checking

## 🔧 Simulation Setup Instructions

### Step 1: Create Wokwi Project

1. Visit [Wokwi Arduino Simulator](https://wokwi.com/arduino)
2. Create new Arduino Uno project
3. Replace default code with simulation-compatible DUYinoboy
4. **Pro Tip**: Sign up for free account to save your projects

### Step 2: Add Virtual Components

```json
{
  "version": 1,
  "author": "DUYinoboy Simulation",
  "editor": "wokwi",
  "parts": [
    {
      "type": "wokwi-arduino-uno",
      "id": "uno1",
      "top": 0,
      "left": 0
    },
    {
      "type": "wokwi-led",
      "id": "led1", 
      "top": 100,
      "left": 200,
      "attrs": {"color": "red"}
    },
    {
      "type": "wokwi-resistor",
      "id": "r1",
      "top": 100,
      "left": 300,
      "attrs": {"value": "220"}
    }
  ],
  "connections": [
    ["uno1:2", "led1:A", "green"],
    ["led1:C", "r1:1", "green"], 
    ["r1:2", "uno1:GND", "black"],
    ["uno1:3", "uno1:A0", "blue"]
  ]
}
```

### Step 3: Simulation-Compatible Code

Create this modified version for simulation:

```cpp
// DUYinoboy Simulation Version
// Compatible with Wokwi and standard Arduino

// Simulate RK-002 functions
#define RK002_GPIO_OUTPUT OUTPUT
#define RK002_GPIO_INPUT_PULLUP INPUT_PULLUP

void RK002_setGPIOType(int pin, int mode) {
  pinMode(pin, mode);
}

void RK002_writeGPIO(int pin, int value) {
  digitalWrite(pin, value);
}

// Pin definitions for simulation
#define GB_SIN 3          // Digital pin 3 = Game Boy SIN
#define STATUS_LED_PIN 2  // Digital pin 2 = Status LED
#define MIDI_INPUT_PIN A0 // Analog pin 0 = MIDI simulation

// Core DUYinoboy variables
byte currentMode = 0;
boolean clockRunning = false;
unsigned long clockTimer = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("DUYinoboy Simulation Started");
  
  // Initialize pins
  RK002_setGPIOType(GB_SIN, RK002_GPIO_OUTPUT);
  RK002_setGPIOType(STATUS_LED_PIN, RK002_GPIO_OUTPUT);
  pinMode(MIDI_INPUT_PIN, INPUT);
  
  // Set initial states
  RK002_writeGPIO(GB_SIN, HIGH);
  RK002_writeGPIO(STATUS_LED_PIN, LOW);
  
  // Startup flash
  flashLED(3);
}

void loop() {
  // Simulate MIDI input via analog pin
  int midiSim = analogRead(MIDI_INPUT_PIN);
  
  // Process simulated MIDI commands
  if (midiSim > 512) {  // High value = note on
    simulateNoteOn(60, 127);  // Middle C, max velocity
    delay(100);
  }
  
  // Handle master clock
  if (currentMode == 0 && clockRunning) {
    handleMasterClock();
  }
  
  // Status blink
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    lastBlink = millis();
    RK002_writeGPIO(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
}

void simulateNoteOn(byte note, byte velocity) {
  Serial.print("Note On: ");
  Serial.print(note);
  Serial.print(" Velocity: ");
  Serial.println(velocity);
  
  // Send to "Game Boy" (visualized via serial)
  sendGameBoyByte(0x90);  // Note on status
  sendGameBoyByte(note);
  sendGameBoyByte(velocity);
}

void sendGameBoyByte(byte data) {
  Serial.print("GB Data: 0x");
  Serial.println(data, HEX);
  
  // Visualize bit transmission on GB_SIN pin
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    RK002_writeGPIO(GB_SIN, bitValue ? HIGH : LOW);
    delayMicroseconds(500);  // ArduinoBoy timing
  }
  
  RK002_writeGPIO(GB_SIN, HIGH);  // Idle state
}

void flashLED(byte count) {
  for (byte i = 0; i < count; i++) {
    RK002_writeGPIO(STATUS_LED_PIN, HIGH);
    delay(100);
    RK002_writeGPIO(STATUS_LED_PIN, LOW);
    delay(100);
  }
}

void handleMasterClock() {
  unsigned long currentTime = millis();
  if (currentTime - clockTimer >= 125) {  // 120 BPM @ 4 PPQN
    clockTimer = currentTime;
    
    // Send clock pulse
    RK002_writeGPIO(GB_SIN, LOW);
    delayMicroseconds(50);
    RK002_writeGPIO(GB_SIN, HIGH);
    
    Serial.println("Clock Pulse");
  }
}
```

## 📊 Testing Scenarios

**Quick Start Links**:
- [Example Wokwi Project](https://wokwi.com/projects/new/arduino-uno) - Start here
- [Arduino Reference](https://www.arduino.cc/reference/en/) - Language documentation
- [MIDI Tutorial](https://itp.nyu.edu/physcomp/labs/labs-serial-communication/midi/) - Understanding MIDI basics

### 1. GPIO Timing Test
Monitor pin 3 (GB_SIN) with oscilloscope to verify:
- 500μs bit timing
- Proper idle states
- Clock pulse generation

### 2. MIDI Simulation
Use analog input to simulate MIDI commands:
- 0-256: Note Off
- 512-768: Note On
- 768-1023: Clock/Transport

### 3. Mode Switching
Test mode changes via serial commands:
```cpp
void serialEvent() {
  if (Serial.available()) {
    byte mode = Serial.parseInt();
    if (mode < 8) {
      currentMode = mode;
      Serial.print("Mode changed to: ");
      Serial.println(mode);
    }
  }
}
```

## 🎯 Validation Tests

### Test 1: Basic GPIO Function
```cpp
void testGPIO() {
  Serial.println("Testing GPIO...");
  
  // Test output
  RK002_writeGPIO(GB_SIN, HIGH);
  delay(100);
  RK002_writeGPIO(GB_SIN, LOW);
  delay(100);
  
  Serial.println("GPIO test complete");
}
```

### Test 2: Timing Accuracy
```cpp
void testTiming() {
  Serial.println("Testing timing...");
  
  unsigned long start = micros();
  sendGameBoyByte(0xAA);  // 10101010 pattern
  unsigned long duration = micros() - start;
  
  Serial.print("Byte transmission time: ");
  Serial.print(duration);
  Serial.println(" microseconds");
  Serial.print("Expected: ~4000 microseconds");
}
```

### Test 3: MIDI Processing
```cpp
void testMIDI() {
  Serial.println("Testing MIDI processing...");
  
  // Simulate various MIDI messages
  simulateNoteOn(60, 127);   // Middle C
  delay(500);
  simulateNoteOn(64, 100);   // E
  delay(500);
  simulateNoteOn(67, 80);    // G
  
  Serial.println("MIDI test complete");
}
```

## 📈 Performance Monitoring

### Wokwi Logic Analyzer Setup
1. Click "+" to add Logic Analyzer
2. Connect to pins 2 and 3
3. Set sample rate to 1MHz
4. Trigger on pin 3 falling edge

### Expected Patterns
- **LSDJ Clock**: 125ms intervals (120 BPM)
- **Note Data**: 8-bit patterns at 500μs/bit
- **Status LED**: 1Hz blink when idle

## 🔍 Troubleshooting Simulation

### Common Issues
1. **Timing too fast in simulation**: Increase delay values
2. **No GPIO response**: Check pin connections in simulator
3. **Serial output garbled**: Verify baud rate (9600)

### Debug Tips
- Use Serial.println() extensively
- Monitor timing with oscilloscope
- Test one mode at a time
- Verify pin states visually

## 🚀 Advanced Simulation

### Virtual Game Boy Response
Create a simple Game Boy simulator that responds to DUYinoboy:

```cpp
// Add to main simulation code
void simulateGameBoyResponse() {
  // Monitor GB_SIN pin for incoming data
  // Decode bytes and show "Game Boy" reactions
  static byte receivedByte = 0;
  static byte bitCount = 0;
  
  // This would be implemented with interrupt timing
  // in a real simulation environment
}
```

## 🔗 Additional Resources

### Online Learning
- [Arduino Getting Started Guide](https://www.arduino.cc/en/Guide)
- [MIDI Programming Tutorial](https://itp.nyu.edu/physcomp/labs/labs-serial-communication/midi/)
- [Game Boy Hardware Documentation](https://gbdev.io/pandocs/)
- [Wokwi Documentation](https://docs.wokwi.com/)

### Community Support
- [Arduino Forum](https://forum.arduino.cc/) - General Arduino help
- [Retrokits Community](https://retrokits.com/) - RK-002 specific support
- [LSDJ Community](https://littlesounddj.fandom.com/wiki/Little_Sound_Dj_Wiki)
- [Chip Music Forums](https://chipmusic.org/) - Game Boy music community

This simulation setup allows comprehensive testing of DUYinoboy functionality without requiring actual RK-002 hardware or Game Boy connections. Perfect for learning, prototyping, and debugging before building the physical hardware.