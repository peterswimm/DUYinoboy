# DUY Programming Guide

Complete guide to programming the RK-002 with DUYinoboy firmware.

## DUY vs Arduino IDE

| Feature | DUY Web Editor | Arduino IDE 1.8.x |
|---------|----------------|-------------------|
| **Environment** | Web browser | Desktop app |
| **Setup** | No installation | Requires setup |
| **Programming** | Via MIDI only | Via MIDI only |
| **Code Storage** | Cloud + sharing | Local files |
| **Compilation** | Cloud-based | Local computer |
| **Best For** | Beginners, sharing | Advanced users |

## Method 1: DUY Web Editor (Recommended)

### Setup
1. **Visit**: https://duy.retrokits.com/
2. **Register**: Create account for code editing
3. **Connect RK-002**: MIDI OUT → RK-002 MIDI IN
4. **Install DUY Base**: Upload DUY firmware first

### Creating Your DUYinoboy
```cpp
#include <RK002.h>

// Required metadata
RK002_DECLARE_INFO(
  "DUYinoboy",
  "your-email@domain.com", 
  "1.0",
  "your-unique-uuid-here"
);

// MIDI event handlers
boolean RK002_onNoteOn(byte channel, byte key, byte velocity) {
  // Send note to Game Boy
  sendGameBoyNote(channel, key, velocity);
  return true;
}

boolean RK002_onNoteOff(byte channel, byte key, byte velocity) {
  // Handle note off
  sendGameBoyNoteOff(channel, key);
  return true;
}

// Setup and loop
void setup() {
  // Initialize GPIO for Game Boy
  RK002_setGPIOType(1, RK002_GPIO_OUTPUT);  // SIN
  RK002_setGPIOType(2, RK002_GPIO_INPUT_PULLUP);  // SOUT
}

void loop() {
  // Handle timing and status updates
}
```

### Uploading to RK-002
1. **Compile**: Click "Compile" in DUY editor
2. **Connect**: Ensure RK-002 connected to MIDI IN
3. **Upload**: Click "Upload to RK-002"
4. **Verify**: Check for success message

## Method 2: Arduino IDE 1.8.x

### IDE Setup
1. **Install Arduino IDE 1.8.x** (NOT 2.x - incompatible)
2. **Add Board Package**:
   - File → Preferences 
   - Additional Boards Manager URLs: 
     `https://www.retrokits.com/rk002/arduino/package_retrokits_index.json`
3. **Install RK-002 Board**:
   - Tools → Board → Boards Manager
   - Search "RK002" and install

### Project Setup
1. **Select Board**: Tools → Board → RK-002
2. **Select Port**: Tools → Port → [Your MIDI Interface]
3. **Create Sketch**: Use DUYinoboy template

### Programming Process
1. **Connect Hardware**: MIDI OUT → RK-002 MIDI IN
2. **Verify Code**: Click verify button (✓)
3. **Upload**: Click upload button (→)
4. **Monitor**: Check IDE messages for success

## RK-002 Specific Programming

### GPIO Functions
```cpp
// Set GPIO pin type (in setup() only)
RK002_setGPIOType(pin, type);

// Types available:
RK002_GPIO_OUTPUT
RK002_GPIO_INPUT
RK002_GPIO_INPUT_PULLUP

// Write to GPIO pin
RK002_writeGPIO(pin, value);  // HIGH or LOW

// Read from GPIO pin  
boolean state = RK002_readGPIO(pin);
```

### MIDI Functions
```cpp
// MIDI input callbacks (return true to pass through)
boolean RK002_onNoteOn(byte channel, byte key, byte velocity);
boolean RK002_onNoteOff(byte channel, byte key, byte velocity);
boolean RK002_onControlChange(byte channel, byte cc, byte value);
boolean RK002_onProgramChange(byte channel, byte program);
boolean RK002_onClock();
boolean RK002_onStart();
boolean RK002_onStop();
boolean RK002_onContinue();

// MIDI output functions (only if MIDI OUT connected)
RK002_sendNoteOn(channel, key, velocity);
RK002_sendNoteOff(channel, key, velocity);
RK002_sendControlChange(channel, cc, value);
RK002_sendProgramChange(channel, program);
```

### Memory Management
```cpp
// Use PROGMEM for constant data
const byte PROGMEM lsdj_note_table[] = {
  0x00, 0x01, 0x02, 0x03  // Your lookup table
};

// Read from PROGMEM
byte note = pgm_read_byte(&lsdj_note_table[index]);

// Minimize RAM usage
static byte modeState = 0;  // Use static instead of global
```

### Timing Functions
```cpp
// Use millis() for timing (never delay() in callbacks)
unsigned long currentTime = millis();
if (currentTime - lastTime >= interval) {
  // Do timed action
  lastTime = currentTime;
}

// Microsecond delays (use sparingly)
delayMicroseconds(62);  // Game Boy bit timing
```

## Game Boy Communication

### Basic Serial Protocol
```cpp
void sendGameBoyByte(byte data) {
  for (int bit = 7; bit >= 0; bit--) {
    boolean bitValue = (data >> bit) & 1;
    
    // Set data line
    RK002_writeGPIO(1, bitValue ? HIGH : LOW);
    
    // Game Boy timing
    delayMicroseconds(31);  // Half bit time
    
    // Clock pulse simulation  
    RK002_writeGPIO(1, LOW);
    delayMicroseconds(2);
    RK002_writeGPIO(1, bitValue ? HIGH : LOW);
    
    delayMicroseconds(31);  // Complete bit
  }
  
  // Return to idle
  RK002_writeGPIO(1, HIGH);
}
```

### LSDJ Protocol
```cpp
void sendLSDJNote(byte channel, byte note, byte velocity) {
  sendGameBoyByte(0x90 | (channel & 0x0F));  // Note on + channel
  sendGameBoyByte(note);                     // MIDI note
  sendGameBoyByte(velocity >> 3);            // Velocity (0-15)
}

void sendLSDJClock() {
  // LSDJ expects clock pulses at 4 PPQN
  RK002_writeGPIO(1, LOW);
  delayMicroseconds(10);
  RK002_writeGPIO(1, HIGH);
}
```

## Debugging Techniques

### LED Status Indication
```cpp
#define STATUS_LED_PIN 2

void flashError(byte count) {
  for (byte i = 0; i < count; i++) {
    RK002_writeGPIO(STATUS_LED_PIN, HIGH);
    delay(200);
    RK002_writeGPIO(STATUS_LED_PIN, LOW);
    delay(200);
  }
}
```

### MIDI Debug Output
```cpp
void debugValue(byte value) {
  // Send debug info via MIDI CC (if MIDI OUT available)
  RK002_sendControlChange(16, 120, value);
}
```

### Game Boy Visual Debug
```cpp
void sendDebugPattern() {
  // Send pattern to Game Boy for visual confirmation
  sendGameBoyByte(0xAA);  // Alternating bit pattern
  sendGameBoyByte(0x55);
}
```

## Common Issues

### Upload Fails
- **MIDI Interface**: Must support SysEx messages
- **Power**: RK-002 must be powered by MIDI port
- **Connection**: Check MIDI OUT → RK-002 MIDI IN
- **Drivers**: Ensure MIDI interface drivers installed

### Code Compilation Errors
```cpp
// Common fixes:
#include <RK002.h>  // Must be first include

// Use byte instead of int for MIDI values
byte channel = 1;   // ✓ Correct
int channel = 1;    // ✗ Avoid

// Return boolean from MIDI callbacks
boolean RK002_onNoteOn(...) {
  return true;  // ✓ Must return boolean
}
```

### Runtime Issues
- **GPIO Not Working**: Set pin type in setup()
- **Timing Problems**: Avoid long delays in callbacks
- **Memory Issues**: Use PROGMEM for constants
- **Game Boy No Response**: Check connections and timing

## Best Practices

### Code Organization
```cpp
// Use clear function names
void handleLSDJKeyboardMode();
void processNanoloopSync();
void updateGameBoyStatus();

// Group related functions
// ============================================================================
// GAME BOY COMMUNICATION
// ============================================================================

// ============================================================================  
// MIDI EVENT HANDLERS
// ============================================================================
```

### Performance
- Keep MIDI callbacks fast (< 1ms)
- Use lookup tables for note mapping
- Minimize dynamic memory allocation
- Cache frequently used values

### Maintenance
- Use clear variable names
- Comment timing-critical sections
- Version your firmware
- Document any hardware dependencies

## Example Projects

See the `examples/` directory for:
- **Basic LSDJ Sync** - Simple master/slave sync
- **Note Mapper** - MIDI note to Game Boy channel mapping
- **Chord Mode** - Play chords on Game Boy
- **Arpeggiator** - Built-in arp functionality

## Resources

- **DUY Portal**: https://duy.retrokits.com/
- **RK-002 Documentation**: https://retrokits.com/support/
- **Arduino Reference**: https://www.arduino.cc/reference/
- **Game Boy Dev**: https://gbdev.io/
