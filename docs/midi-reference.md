# MIDI Control Reference

Complete MIDI implementation for DUYinoboy modes and controls.

**References**:
- [MIDI Beat Clock Standard](https://en.wikipedia.org/wiki/MIDI_beat_clock) - 24 PPQN specification
- [MIDI PPQN Technical Guide](https://www.sweetwater.com/insync/ppqn-pulses-per-quarter-note-parts-per-quarter-note/)
- [LSDJ MIDI Documentation](https://www.littlesounddj.com/lsd/latest/documentation/LSDj_9_2_6.pdf)
- [ArduinoBoy MIDI Reference](https://github.com/trash80/Arduinoboy)

## Mode Selection

**Note**: DUYinoboy is MIDI input only - all modes follow external MIDI clock

### Primary Method: Program Change
| Program | Mode | Function | Game Boy Software |
|---------|------|----------|-------------------|
| 0 | LSDJ MIDI Clock Sync | Follow external MIDI clock | LSDJ (slave sync mode) |
| 1 | LSDJ Keyboard Mode | Real-time note input | LSDJ Keyboard mode |
| 2 | LSDJ Map Mode | 4-channel polyphonic | LSDJ (channels 1-4) |
| 3 | Nanoloop MIDI Clock Sync | Follow external MIDI clock | Nanoloop 1.x (slave sync) |
| 4 | Custom Mode 1 | User-definable | Custom software |
| 5 | Custom Mode 2 | User-definable | Custom software |
| 6 | Custom Mode 3 | User-definable | Custom software |
| 7 | Custom Mode 4 | User-definable | Custom software |

**Example**: Send Program Change 1 to select LSDJ Keyboard mode.

### Alternative: Mod Wheel (CC1)
| CC1 Value | Mode Selected |
|-----------|---------------|
| 0-15 | Mode 0 (LSDJ Clock Sync) |
| 16-31 | Mode 1 (LSDJ Keyboard) |
| 32-47 | Mode 2 (LSDJ Map) |
| 48-63 | Mode 3 (Nanoloop Clock Sync) |
| 64-79 | Mode 4 (Custom 1) |
| 80-95 | Mode 5 (Custom 2) |
| 96-111 | Mode 6 (Custom 3) |
| 112-127 | Mode 7 (Custom 4) |

## Note Messages

### Note On/Off
**All Modes**: Notes trigger Game Boy sounds based on current mode.

| Mode | Note Behavior |
|------|---------------|
| LSDJ Keyboard | Single channel, real-time play |
| LSDJ Map | Polyphonic across 4 channels |
| Nanoloop Clock Sync | Trigger samples/steps |
| Custom | User-defined behavior |

**MIDI Format**:
- Note On: `90 [note] [velocity]`
- Note Off: `80 [note] 00`

**Note Range**: C-2 to G8 (MIDI notes 0-127)

## Control Changes (CC)

### CC1 - Mod Wheel (Alternative Mode Select)
- **Range**: 0-127
- **Function**: Select mode (see table above)
- **Usage**: Alternative to Program Change

### CC7 - Volume (Reserved - Not Used)
- **Range**: 0-127
- **Function**: Reserved (RK-002 follows external MIDI clock only)
- **Note**: BPM controlled by external DAW/sequencer sending MIDI clock

### CC16 - General Purpose 1 (MIDI Channel)
- **Range**: 0-127 (maps to MIDI channels 1-16)
- **Function**: Set MIDI input channel for single-channel modes
- **Active Modes**: LSDJ Keyboard, Nanoloop Clock Sync mode
- **Auto-Save**: Settings saved to EEPROM immediately

### CC17-20 - General Purpose 2-5 (LSDJ Map Channels)
| CC | Function | Range |
|----|----------|-------|
| 17 | LSDJ Channel 1 MIDI input | 1-16 |
| 18 | LSDJ Channel 2 MIDI input | 1-16 |
| 19 | LSDJ Channel 3 MIDI input | 1-16 |
| 20 | LSDJ Channel 4 MIDI input | 1-16 |

**Active Mode**: LSDJ Map only
**Default**: Channels 1, 2, 3, 4 respectively
**Auto-Save**: Settings saved to EEPROM immediately

### CC21 - General Purpose 6 (Velocity Curve)
- **Range**: 0-127
- **Function**: Select velocity response curve
- **Values**:
  - 0-31: Linear
  - 32-63: Logarithmic  
  - 64-95: Exponential
  - 96-127: S-curve

### CC64 - Sustain Pedal (Transport Control)
- **Range**: 0-127
- **Function**: Start/Stop sequencer
- **Values**:
  - 0-63: Stop
  - 64-127: Start
- **Active Modes**: All modes

## Real-Time Messages

### MIDI Clock (F8)
- **Standard**: [MIDI Beat Clock](https://en.wikipedia.org/wiki/MIDI_beat_clock) - 24 PPQN
- **Function**: Clock sync input for external synchronization
- **Active Modes**: LSDJ MIDI Clock Sync, Nanoloop MIDI Clock Sync
- **Conversion**: 24 PPQN MIDI → 4 PPQN Game Boy (6:1 division ratio)
- **Timing**: 60,000,000 / (24 × BPM) microseconds between clocks

### Transport Commands

#### Start (FA)
- **Function**: Start sequencer from beginning
- **Active Modes**: Clock sync modes

#### Stop (FC)  
- **Function**: Stop sequencer
- **Active Modes**: All modes

#### Continue (FB)
- **Function**: Resume from current position
- **Active Modes**: Clock sync modes

## Mode-Specific MIDI Implementation

### LSDJ MIDI Clock Sync Mode (0)
**MIDI Input**:
- MIDI Clock: Follow external clock
- Start/Stop/Continue: Transport control
- CC64: Start/Stop transport

**Behavior**:
- Follows incoming MIDI clock
- Converts 24 PPQN to 4 PPQN for LSDJ
- No internal clock generation

### LSDJ Keyboard Mode (1)
**MIDI Input**:
- Note On/Off: Real-time note play
- CC16: MIDI channel select (1-16)

**Behavior**:
- Single-channel monophonic play
- Direct MIDI-to-LSDJ note conversion
- Velocity affects LSDJ instrument parameters

### LSDJ Map Mode (2)
**MIDI Input**:
- Note On/Off: 4-channel polyphonic
- CC17-20: Channel MIDI mapping

**Behavior**:
- Each MIDI channel maps to LSDJ channel
- Up to 4-note polyphony
- Independent channel control

### Nanoloop MIDI Clock Sync Mode (3)
**MIDI Input**:
- MIDI Clock: Follow external clock
- Note On: Trigger samples
- Start/Stop/Continue: Transport control

**Behavior**:
- Follows incoming MIDI clock
- Notes trigger sample steps
- No internal clock generation

## MIDI Channel Usage

### Default Channel Assignment
- **Input Channel**: 1 (adjustable via CC16)
- **Map Mode Channels**: 1, 2, 3, 4 (adjustable via CC17-20)

### Channel Filtering
```
Single Channel Modes (Keyboard, Nanoloop):
- Listen only to selected MIDI channel
- Ignore other channels

Multi-Channel Modes (Map):
- Listen to mapped channels only
- Each Game Boy channel has its own MIDI channel
```

## Velocity Handling

### LSDJ Velocity Mapping
```
MIDI Velocity (0-127) → LSDJ Velocity (1-15)
Formula: lsdj_vel = (midi_vel >> 3) + 1
Special: velocity 0 = LSDJ velocity 0 (note off)
```

### Nanoloop Velocity
```
MIDI Velocity (0-127) → Nanoloop Velocity (0-31)
Formula: nano_vel = midi_vel >> 2
Higher velocity = louder sample playback
```

## Persistent Settings

DUYinoboy automatically saves the following settings to EEPROM:
- Current mode (Program Change)
- MIDI input channel (CC16)
- BPM setting (CC7)
- LSDJ Map channel assignments (CC17-20)

Settings are restored on power-up.

## System Exclusive (SysEx)

DUYinoboy does not implement custom SysEx commands. All configuration is done via standard MIDI CC messages.

## MIDI Implementation Chart

| Function | Transmitted | Recognized | Remarks |
|----------|-------------|------------|---------|
| Basic Channel | N/A | 1-16 | Configurable via CC16, saved to EEPROM |
| Mode | N/A | Mode 3 (Omni Off) | |
| Note Number | N/A | 0-127 | |
| Velocity | N/A | Note On: 1-127<br>Note Off: 0 | |
| After Touch | N/A | No | |
| Pitch Bend | N/A | No | |
| Control Change | N/A | 1, 7, 16-21, 64 | See CC table |
| Program Change | N/A | 0-7 | Mode selection |
| System Exclusive | N/A | No | |
| System Common | N/A | No | |
| System Real Time | N/A | Clock, Start, Stop, Continue | Slave modes only |
| Aux Messages | N/A | No | |

## Troubleshooting

### No Response to MIDI
- Check MIDI channel assignment (CC16)
- Verify mode selection (Program Change 0-7)
- Ensure proper MIDI cable connection

### Wrong Notes/Timing
- Verify Game Boy software is ready
- Check MIDI note range compatibility
- Test with different velocity values

### Clock Sync Issues
- Ensure slave mode selected (Program 1 or 5)
- Check MIDI clock is being sent
- Verify transport commands (Start/Stop)

### Mode Changes Not Working
- Try Program Change instead of CC1
- Check value ranges for CC1 (0-127)
- Power cycle RK-002 if stuck

## Example MIDI Sequences

### Basic Setup
```
C0 02                 ; Program Change 2 - Select LSDJ Keyboard mode
B0 10 00              ; CC16 value 0 - Set MIDI channel 1
B0 07 7F              ; CC7 value 127 - Set BPM to ~200
```

### Play Note
```
90 3C 7F              ; Note On: Middle C, velocity 127
80 3C 00              ; Note Off: Middle C
```

### Start Clock Sync
```
C0 01                 ; Program Change 1 - Select LSDJ Slave mode
FA                    ; MIDI Start (real-time message)
F8 F8 F8...           ; MIDI Clock at 24 PPQN (real-time messages)
FC                    ; MIDI Stop (when needed)
```
**Timing**: At 120 BPM, MIDI clocks occur every 20.83ms (60,000,000 ÷ (24 × 120))
