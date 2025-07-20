# Troubleshooting Guide

Common issues and solutions for DUYinoboy setup and operation.

## Hardware Issues

### RK-002 Not Programming

**Symptoms**: Arduino IDE shows upload errors or DUY portal fails to connect

**Causes & Solutions**:

1. **MIDI Interface Incompatible**
   - **Check**: Does your MIDI interface support SysEx messages?
   - **Test**: Try with known compatible interface (RK-005, RK-006)
   - **Alternative**: Use different USB-MIDI device

2. **Power Issues**  
   - **Check**: Is RK-002 getting power from MIDI input?
   - **Test**: LED should be visible when connected
   - **Fix**: Use [powered MIDI interface](https://retrokits.com/shop/rk005/) or MIDI thru box
   - **Note**: RK-002 is MIDI input only, no MIDI output capability
   - **Reference**: [RK-002 Power Requirements](https://retrokits.com/shop/rk002/)

3. **Wrong Arduino IDE Version**
   - **Check**: Must use [Arduino IDE 1.8.x](https://www.arduino.cc/en/software) (NOT 2.x)
   - **Fix**: Download 1.8.19 from [Arduino.cc archives](https://www.arduino.cc/en/software)
   - **Note**: RK-002 board package incompatible with IDE 2.x

4. **Incorrect Board Selection**
   - **Check**: Tools → Board → RK-002 selected?
   - **Fix**: Install [RK-002 board package](https://retrokits.com/rk002-duy/) first
   - **URL**: `https://www.retrokits.com/rk002/arduino/package_retrokits_index.json`

### Game Boy Not Responding

**Symptoms**: No sound, sync, or response from Game Boy

**Hardware Checks**:

1. **Cable Connections**
   ```
   ✓ Game Boy Pin 3 (SIN) → RK-002 GPIO 1
   ✓ Game Boy Pin 6 (GND) → RK-002 Ground
   ✓ Solid solder joints
   ✓ No short circuits
   ```

2. **Wire Color Verification**
   - **Don't trust colors** - Always test with multimeter
   - **Test continuity** from Game Boy pin to RK-002 pin
   - **Common mistake**: Orange and red wires swapped

3. **Game Boy Software Ready**
   - **LSDJ**: Set sync mode to MIDI
   - **Nanoloop**: Set sync mode to slave
   - **Power on** Game Boy before sending MIDI

4. **Voltage Issues**
   - **Game Boy**: 5V signal levels
   - **RK-002**: 3.3V GPIO pins (usually compatible)
   - **If problems**: Add level shifter circuit

### Intermittent Connection

**Symptoms**: Works sometimes, fails randomly

**Causes & Solutions**:

1. **Loose Connections**
   - **Check**: Wiggle test all solder joints
   - **Fix**: Re-solder suspect connections
   - **Prevent**: Use strain relief on wires

2. **Ground Loops**  
   - **Check**: Multiple ground paths between devices
   - **Fix**: Use single ground connection point
   - **Test**: Disconnect unnecessary grounds

3. **Power Supply Noise**
   - **Check**: Switch-mode power supplies can cause issues
   - **Fix**: Use linear power supply for audio gear
   - **Test**: Try battery power for Game Boy

## Software Issues

### Wrong Notes Playing

**Symptoms**: Game Boy plays different notes than expected

**Debugging Steps**:

1. **Note Mapping Issues**
   ```cpp
   // Check your note conversion function
   byte mapMIDIToLSDJNote(byte midiNote) {
     // Ensure this returns correct LSDJ note value
     return midiNote; // Simple 1:1 mapping
   }
   ```

2. **Channel Assignment**
   - **Check**: MIDI channel matches expected channel
   - **Test**: Send to channel 1 explicitly
   - **Debug**: Use CC16 to set channel

3. **Velocity Scaling**
   ```cpp
   // LSDJ expects 0-15 velocity range
   byte lsdjVelocity = midiVelocity >> 3; // Divide by 8
   ```

### Timing Issues

**Symptoms**: Notes too fast, too slow, or choppy

**Common Causes**:

1. **Clock Division Wrong**
   ```cpp
   // MIDI = 24 PPQN, LSDJ = 4 PPQN
   #define CLOCK_DIVISION 6  // 24/4 = 6
   ```

2. **Timing Delays Too Long**
   ```cpp
   // Keep MIDI callbacks fast
   boolean RK002_onNoteOn(...) {
     // Do work quickly
     return true; // Don't delay here
   }
   ```

3. **BPM Calculation**
   ```cpp
   // For 120 BPM, 4 PPQN:
   // 120 BPM = 2 beats/sec = 8 ticks/sec = 125ms per tick
   unsigned long interval = 60000UL / (bpm * 4);
   ```

### Mode Changes Not Working  

**Symptoms**: Program changes ignored, stuck in one mode

**Solutions**:

1. **Check Program Change Range**
   ```cpp
   boolean RK002_onProgramChange(byte channel, byte program) {
     if (program < NUM_MODES) { // Must be 0-7
       setMode(program);
     }
     return true;
   }
   ```

2. **Try Alternative Method**
   - Use CC1 (Mod Wheel) instead of Program Change
   - Send CC1 values 0, 16, 32, 48, 64, 80, 96, 112

3. **Power Cycle**
   - Disconnect RK-002 from MIDI
   - Wait 5 seconds  
   - Reconnect and try again

## MIDI Issues

### No MIDI Response

**Symptoms**: RK-002 doesn't respond to any MIDI

**Debugging**:

1. **MIDI Connection**
   - **Check**: MIDI controller → RK-002 MIDI IN
   - **Test**: LED should light when MIDI connected
   - **Verify**: Use MIDI monitor software

2. **MIDI Channel**
   - **Default**: Channel 1
   - **Change**: Send CC16 with value 0-15 for channels 1-16
   - **Test**: Send notes on multiple channels

3. **MIDI Interface Issues**
   - **Driver**: Ensure MIDI interface drivers installed
   - **Test**: Try with different MIDI software
   - **Alternative**: Use different MIDI interface

### Wrong MIDI Channel

**Symptoms**: Notes work on some channels but not others

**Solutions**:

1. **Set Channel Explicitly**
   ```
   Send: CC16 value 0 (for MIDI channel 1)
   Send: CC16 value 15 (for MIDI channel 16)
   ```

2. **Check Mode Requirements**
   - **Single modes**: Use one channel
   - **Map mode**: Use channels set by CC17-20

## Game Boy Software Issues

### LSDJ Not Syncing

**Required LSDJ Settings**:
- **Sync**: Set to MIDI (not OFF)
- **MIDI**: Enable in project settings  
- **Channel**: Match your MIDI channel
- **Reference**: [LSDJ MIDI Sync Guide](https://littlesounddj.fandom.com/wiki/Multiboot_MIDI_Sync)

**Steps**:
1. Load LSDJ
2. Press SELECT to enter project screen
3. Navigate to SYNC settings
4. Set SYNC to MIDI
5. Set MIDI channel to 1 (or your chosen channel)

### Nanoloop Not Responding

**Required Nanoloop Settings**:
- **Sync**: Set to slave (not internal)
- **MIDI**: Enable if available

**Steps**:
1. Enter Nanoloop setup
2. Find sync settings
3. Set to external/slave sync
4. Test with simple clock sync first

### mGB Not Loading

**Common Issues**:
- **Flash Cart**: Ensure mGB ROM properly flashed
- **Compatibility**: Check Game Boy model compatibility
- **File**: Use correct mGB version for your Game Boy

## Diagnostic Tools

### Hardware Testing

1. **Multimeter Tests**
   ```
   Continuity: Game Boy pin → RK-002 pin
   Voltage: RK-002 GPIO should be ~3.3V when HIGH
   Resistance: No shorts between different pins
   ```

2. **LED Status Indicator**
   ```cpp
   #define USE_GPIO_LED
   #define STATUS_LED_PIN 2
   
   // Add visual feedback for debugging
   void flashError(byte errorCode) {
     for (byte i = 0; i < errorCode; i++) {
       RK002_writeGPIO(STATUS_LED_PIN, HIGH);
       delay(200);
       RK002_writeGPIO(STATUS_LED_PIN, LOW);
       delay(200);
     }
   }
   ```

### Software Testing

1. **Simple Test Program**
   ```cpp
   // Minimal test - just flash LED on note
   boolean RK002_onNoteOn(byte channel, byte key, byte velocity) {
     RK002_writeGPIO(STATUS_LED_PIN, HIGH);
     delay(100);
     RK002_writeGPIO(STATUS_LED_PIN, LOW);
     return true;
   }
   ```

2. **MIDI Monitor**
   - Use MIDI monitoring software
   - Verify messages are being sent
   - Check timing and values

## Getting Help

### Community Resources
- **[GitHub Issues](https://github.com/DUYinoboy/DUYinoboy/issues)**: Report bugs and ask questions
- **[Retrokits Support](https://retrokits.com/)**: Official hardware support
- **[LSDJ Community](https://littlesounddj.fandom.com/wiki/Little_Sound_Dj_Wiki)**: Software-specific help
- **[Chip Music Forums](https://chipmusic.org/)**: Game Boy music community
- **[Arduino Forum](https://forum.arduino.cc/)**: General Arduino programming help

### Information to Include
When asking for help, provide:
- **Hardware**: Game Boy model, MIDI interface used
- **Software**: LSDJ/Nanoloop version
- **Code**: Your DUYinoboy sketch (or example used)
- **Symptoms**: Detailed description of what's not working
- **Testing**: What troubleshooting steps you've tried

### Common Solutions Summary

| Problem | Quick Fix |
|---------|-----------|
| Upload fails | Check Arduino IDE 1.8.x, MIDI interface SysEx support |
| No Game Boy response | Verify Pin 3 → GPIO 1, Ground connections |
| Wrong notes | Check note mapping function, MIDI channel |
| No sync | Verify clock division (24→4 PPQN), Game Boy sync mode |
| Mode stuck | Try CC1 instead of Program Change, power cycle |
| Intermittent | Check solder joints, ground loops |

Most issues are hardware connection problems - always start with multimeter testing!
