# MIDI Control Reference

Complete MIDI implementation for DUYinoboy modes and controls.

## Mode Selection

### Primary Method: Program Change
| Program | Mode | Function | Game Boy Software |
|---------|------|----------|-------------------|
| 0 | LSDJ Master | Generate clock for LSDJ | LSDJ (any version) |
| 1 | LSDJ Slave | Sync to external clock | LSDJ (any version) |
| 2 | LSDJ Keyboard | Real-time note input | LSDJ Keyboard mode |
| 3 | LSDJ Map | 4-channel polyphonic | LSDJ (channels 1-4) |
| 4 | Nanoloop Master | Generate clock for Nanoloop | Nanoloop 1.x |
| 5 | Nanoloop Slave | Sync to external clock | Nanoloop 1.x |
| 6 | Custom Mode 1 | User-definable | Custom software |
| 7 | Custom Mode 2 | User-definable | Custom software |

**Example**: Send Program Change 2 to select LSDJ Keyboard mode.

### Alternative: Mod Wheel (CC1)
| CC1 Value | Mode Selected |
|-----------|---------------|
| 0-15 | Mode 0 (LSDJ Master) |
| 16-31 | Mode 1 (LSDJ Slave) |
| 32-47 | Mode 2 (LSDJ Keyboard) |
| 48-63 | Mode 3 (LSDJ Map) |
| 64-79 | Mode 4 (Nanoloop Master) |
| 80-95 | Mode 5 (Nanoloop Slave) |
| 96-111 | Mode 6 (Custom 1) |
| 112-127 | Mode 7 (Custom 2) |

## Note Messages

### Note On/Off
**All Modes**: Notes trigger Game Boy sounds based on current mode.

| Mode | Note Behavior |
|------|---------------|
| LSDJ Keyboard | Single channel, real-time play |
| LSDJ Map | Polyphonic across 4 channels |
| Nanoloop | Trigger samples/steps |
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

### CC7 - Volume (BPM Control)
- **Range**: 0-127 (maps to 60-200 BPM)
- **Function**: Set tempo for master modes
- **Active Modes**: LSDJ Master, Nanoloop Master only
- **Formula**: BPM = 60 + (CC7 * 140/127)

### CC16 - General Purpose 1 (MIDI Channel)
- **Range**: 0-127 (maps to MIDI channels 1-16)
- **Function**: Set MIDI input channel for single-channel modes
- **Active Modes**: LSDJ Keyboard, Nanoloop modes

### CC17-20 - General Purpose 2-5 (LSDJ Map Channels)
| CC | Function | Range |
|----|----------|-------|
| 17 | LSDJ Channel 1 MIDI input | 1-16 |
| 18 | LSDJ Channel 2 MIDI input | 1-16 |
| 19 | LSDJ Channel 3 MIDI input | 1-16 |
| 20 | LSDJ Channel 4 MIDI input | 1-16 |

**Active Mode**: LSDJ Map only
**Default**: Channels 1, 2, 3, 4 respectively

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

### MIDI Clock
- **Function**: Slave sync input (24 PPQN)
- **Active Modes**: LSDJ Slave, Nanoloop Slave
- **Conversion**: 24 PPQN MIDI → 4 PPQN Game Boy

### Transport Commands

#### Start (FA)
- **Function**: Start sequencer from beginning
- **Active Modes**: Slave modes

#### Stop (FC)  
- **Function**: Stop sequencer
- **Active Modes**: All modes

#### Continue (FB)
- **Function**: Resume from current position
- **Active Modes**: Slave modes

## Mode-Specific MIDI Implementation

### LSDJ Master Mode (0)
**MIDI Input**:
- CC7: Set BPM (60-200)
- CC64: Start/Stop transport

**Behavior**:
- Generates 4 PPQN clock for LSDJ
- BPM controllable via CC7
- Start/stop via CC64

### LSDJ Slave Mode (1)  
**MIDI Input**:
- MIDI Clock: Follow external clock
- Start/Stop/Continue: Transport control

**Behavior**:
- Follows incoming MIDI clock
- Converts 24 PPQN to 4 PPQN for LSDJ

### LSDJ Keyboard Mode (2)
**MIDI Input**:
- Note On/Off: Real-time note play
- CC16: MIDI channel select (1-16)

**Behavior**:
- Single-channel monophonic play
- Direct MIDI-to-LSDJ note conversion
- Velocity affects LSDJ instrument parameters

### LSDJ Map Mode (3)
**MIDI Input**:
- Note On/Off: 4-channel polyphonic
- CC17-20: Channel MIDI mapping

**Behavior**:
- Each MIDI channel maps to LSDJ channel
- Up to 4-note polyphony
- Independent channel control

### Nanoloop Master Mode (4)
**MIDI Input**:
- Note On: Trigger samples
- CC7: Set BPM
- CC64: Start/Stop

**Behavior**:
- Generates clock for Nanoloop
- Notes trigger sample steps

### Nanoloop Slave Mode (5)
**MIDI Input**:
- MIDI Clock: Sync input
- Note On: Trigger samples
- Transport: Start/Stop/Continue

**Behavior**:
- Sync to external MIDI clock
- Note mapping to Nanoloop steps

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
MIDI Velocity (0-127) → LSDJ Velocity (0-15)
Formula: lsdj_vel = midi_vel >> 3
```

### Nanoloop Velocity
```
MIDI Velocity → Sample trigger strength
Higher velocity = louder sample playback
```

## System Exclusive (SysEx)

DUYinoboy does not implement custom SysEx commands. All configuration is done via standard MIDI CC messages.

## MIDI Implementation Chart

| Function | Transmitted | Recognized | Remarks |
|----------|-------------|------------|---------|
| Basic Channel | N/A | 1-16 | Via CC16 |
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
F0 Program Change 2    ; Select LSDJ Keyboard mode
B0 10 00              ; Set MIDI channel 1 (CC16 = 0)
B0 07 7F              ; Set BPM to ~200 (CC7 = 127)
```

### Play Note
```
90 3C 7F              ; Note On: Middle C, velocity 127
80 3C 00              ; Note Off: Middle C
```

### Start Clock Sync
```
F0 Program Change 1    ; Select LSDJ Slave mode
FA                    ; MIDI Start
F8 F8 F8...           ; MIDI Clock (24 PPQN)
```
