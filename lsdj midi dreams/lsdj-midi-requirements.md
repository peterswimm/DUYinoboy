# LSDj MIDI Implementation Manual

*A comprehensive MIDI specification for LSDj integration with modern DAWs and hardware*

**Note**: This specification is based on actual LSDj capabilities and real hardware implementations (Arduinoboy, mGB). All MIDI mappings and parameters reflect actual functionality found in source code and official documentation.

## Overview

This document outlines the complete MIDI implementation for LSDj, enabling seamless integration with Digital Audio Workstations (DAWs), MIDI controllers, and modern music production workflows. The implementation supports both note input and parameter automation across all LSDj channels.

## MIDI Channels

LSDj MIDI implementation based on core tracker functionality:

### Core Game Boy Channels (1-4)
- **Channel 1**: Pulse 1 (square wave synthesis, supports sweep)
- **Channel 2**: Pulse 2 (square wave synthesis, no sweep, supports detune)
- **Channel 3**: Wave (wavetable synthesis and sample playback)
- **Channel 4**: Noise (percussion and noise effects)

### Control Channels
- **Channel 5**: Master Mode (global controls, live mode row triggering, Game Boy controls)
- **Channel 6**: Live Mode Extensions (chain triggering, toggle effects)
- **Channel 7**: Polyphonic Mode (global commands across all channels simultaneously)
- **Channel 8**: RetroPlug Integration (VST/AU specific features, DAW automation)

## Note Mapping

### Core Channels (1-4)

#### Channel 1 - Pulse 1 (MIDI Output Mode)
- **Function**: LSDj sends MIDI notes *from* Pulse 1 channel to external devices
- **Note Range**: C2-B8 (MIDI notes 36-107)
- **Commands**: N## (absolute notes), Q## (relative to channel pitch)
- **Features**: Supports sweep functionality
- **Limitations**: Cannot use detune command

#### Channel 2 - Pulse 2 (MIDI Output Mode)  
- **Function**: LSDj sends MIDI notes *from* Pulse 2 channel to external devices
- **Note Range**: C2-B8 (MIDI notes 36-107)
- **Commands**: N## (absolute notes), Q## (relative to channel pitch)
- **Features**: Supports detune command (D command)
- **Limitations**: No sweep functionality

#### Channel 3 - Wave (MIDI Output Mode)
- **Function**: LSDj sends MIDI notes *from* Wave channel to external devices
- **Note Range**: C2-B8 (MIDI notes 36-107)
- **Commands**: N## (absolute notes), Q## (relative to channel pitch)
- **Instrument Types**: SYNTH (wavetables), KIT (samples), SP256 (speech)
- **Features**: Supports both wavetable synthesis and sample playback

#### Channel 4 - Noise (MIDI Output Mode)
- **Function**: LSDj sends MIDI notes *from* Noise channel to external devices
- **Note Range**: Limited to 16 noise frequencies (mapped to MIDI notes)
- **Commands**: N## (absolute notes), Q## (relative to channel pitch)
- **Features**: Percussion and noise effects
- **Limitations**: Not chromatic like other channels

### Control Channels

#### Channel 5 - Master Mode
**Live Mode Row Triggering**:
- **Notes 36-163**: Trigger LSDj song rows 0-127 (36+row_number)
- **Program Change**: Switch between songs/projects

**Game Boy Controls** (First Octave - Notes 0-11):
- **0 (C-1)**: D-Pad Up
- **1 (C#-1)**: D-Pad Down  
- **2 (D-1)**: D-Pad Left
- **3 (D#-1)**: D-Pad Right
- **4 (E-1)**: A Button
- **5 (F-1)**: B Button
- **6 (F#-1)**: Start Button
- **7 (G-1)**: Select Button
- **8 (G#-1)**: Tempo Up
- **9 (A-1)**: Tempo Down
- **10 (A#-1)**: Screen Navigation Up
- **11 (B-1)**: Screen Navigation Down

**Screen Page Navigation** (Second Octave - Notes 12-23):
- **12 (C0)**: Jump to SONG Screen
- **13 (C#0)**: Jump to CHAIN Screen
- **14 (D0)**: Jump to PHRASE Screen
- **15 (D#0)**: Jump to INSTRUMENT Screen
- **16 (E0)**: Jump to TABLE Screen
- **17 (F0)**: Jump to GROOVE Screen
- **18 (F#0)**: Jump to SYNTH Screen
- **19 (G0)**: Jump to WAVE Screen
- **20 (G#0)**: Jump to KIT Screen
- **21 (A0)**: Jump to SPEECH Screen
- **22 (A#0)**: Jump to TEMPO Screen
- **23 (B0)**: Jump to PROJECT Screen

#### Channel 6 - Live Mode Extensions
**Chain Triggering**:
- **Notes 36-99**: Trigger individual chains 0-63 (36+chain_number)

**Toggle Effects** (Notes 100-115):
- **100**: Toggle Mute Pulse 1
- **101**: Toggle Mute Pulse 2
- **102**: Toggle Mute Wave
- **103**: Toggle Mute Noise
- **104**: Toggle Solo Pulse 1
- **105**: Toggle Solo Pulse 2
- **106**: Toggle Solo Wave
- **107**: Toggle Solo Noise
- **108**: Toggle Sync Mode
- **109**: Toggle Live Mode
- **110**: Toggle Chain Mode
- **111**: Reserved
- **112**: Reserved
- **113**: Reserved
- **114**: Reserved
- **115**: Reserved

**Extended Live Mode Row Triggering** (Notes 116-127):
- **Notes 116-127**: Trigger LSDj song rows 128-139 (116-12+128)

#### Channel 7 - Polyphonic Mode
- **Function**: Global commands applied across all active LSDj channels simultaneously
- **Note Range**: C2-B8 (MIDI notes 36-107)
- **Behavior**: Voice stealing for polyphonic sequences
- **Commands**: All LSDj commands applied globally where applicable

#### Channel 8 - RetroPlug Integration
**Project Management** (Notes 36-47):
- **36 (C2)**: Save Project State
- **37 (C#2)**: Load Project State
- **38 (D2)**: Export ROM
- **39 (D#2)**: Export SRAM (.sav)
- **40 (E2)**: Import ROM
- **41 (F2)**: Import SRAM (.sav)
- **42 (F#2)**: Reset Current System
- **43 (G2)**: Duplicate Current System
- **44 (G#2)**: Switch to Next System
- **45 (A2)**: Switch to Previous System
- **46 (A#2)**: Toggle Game Link (Multi-Instance)
- **47 (B2)**: Toggle System Active/Inactive

**Audio Routing** (Notes 48-59):
- **48 (C3)**: Stereo Mixdown Mode
- **49 (C#3)**: Two Channels Per Instance Mode
- **50 (D3)**: Individual Channel Outputs Mode
- **51 (D#3)**: Toggle Instance 1 Output
- **52 (E3)**: Toggle Instance 2 Output
- **53 (F3)**: Toggle Instance 3 Output
- **54 (F#3)**: Toggle Instance 4 Output
- **55 (G3)**: Reserved
- **56 (G#3)**: Reserved
- **57 (A3)**: Reserved
- **58 (A#3)**: Reserved
- **59 (B3)**: Reserved

**MIDI Routing** (Notes 60-71):
- **60 (C4)**: All Channels to All Instances
- **61 (C#4)**: Four Channels Per Instance
- **62 (D4)**: One Channel Per Instance
- **63 (D#4)**: Custom MIDI Routing
- **64 (E4)**: MIDI Thru On/Off
- **65 (F4)**: MIDI Learn Mode
- **66 (F#4)**: Reset MIDI Mappings
- **67 (G4)**: Reserved
- **68 (G#4)**: Reserved
- **69 (A4)**: Reserved
- **70 (A#4)**: Reserved
- **71 (B4)**: Reserved

**Real-time Kit Management** (Notes 72-95):
- **72-87 (C5-D#6)**: Load Kit Slot 0-15
- **88-95 (E6-B6)**: Reserved Kit Operations

## MIDI Control Change (CC) Assignments

*Note: CC assignments map directly to LSDj commands and are only included for commands that actually exist in LSDj*

### Comprehensive CC Assignment Table

| CC# | Ch1 - Pulse 1 | Ch2 - Pulse 2 | Ch3 - Wave | Ch4 - Noise | Ch5 - Master | Ch6 - Live | Ch7 - Global | Ch8 - RetroPlug |
|-----|---------------|---------------|------------|-------------|--------------|------------|--------------|-----------------|
| 1 | P Bend 1st (0-15) | P Bend 1st (0-15) | P Bend 1st (0-15) | P Bend 1st (0-15) | M Volume (0-127) | Mute PU1 (0,127) | Global P 1st (0-15) | - |
| 2 | P Bend 2nd (0-14) | P Bend 2nd (0-14) | P Bend 2nd (0-14) | P Bend 2nd (0-14) | T Tempo (40-295) | Mute PU2 (0,127) | Global P 2nd (0-14) | - |
| 3 | E Env 1st (0-15) | E Env 1st (0-15) | E Volume (0,1,2,3) | E Env 1st (0-15) | G Groove (0-15) | Mute WAV (0,127) | Global E 1st (0-15) | - |
| 4 | E Env 2nd (0,1-7,8,9-15) | E Env 2nd (0,1-7,8,9-15) | - | E Env 2nd (0,1-7,8,9-15) | - | Mute NOI (0,127) | Global E 2nd (0,1-7,8,9-15) | - |
| 5 | O Output (0,1,2,3) | O Output (0,1,2,3) | O Output (0,1,2,3) | O Output (0,1,2,3) | - | Solo PU1 (0,127) | Global O (0,1,2,3) | - |
| 6 | V Vibrato 1st (0-15) | V Vibrato 1st (0-15) | V Vibrato 1st (0-15) | V Vibrato 1st (0-15) | - | Solo PU2 (0,127) | Global V 1st (0-15) | - |
| 7 | E Volume (0-15) | E Volume (0-15) | E Volume (0-15) | E Volume (0-15) | Song Select (0-31) | Solo WAV (0,127) | Global Volume (0-15) | - |
| 8 | V Vibrato 2nd (0-15) | V Vibrato 2nd (0-15) | V Vibrato 2nd (0-15) | V Vibrato 2nd (0-15) | Chain Mode (0,127) | Solo NOI (0,127) | Global Vol (0-127) | - |
| 9 | S Sweep 1st (0-15) | F Tune 1st (0-15) | W Wave 1st (0-15) | S Shape 1st (0-15) | - | Sync Mode (0,127) | Global Tempo (40-295) | - |
| 10 | S Sweep 2nd (0-15) | F Tune 2nd (0-15) | W Wave 2nd (0-15) | S Shape 2nd (0-15) | Global Pan (0-127) | Chain Mode (0,127) | - | - |
| 11 | - | - | F Frame 1st (0-15) | - | - | - | - | - |
| 12 | - | - | F Frame 2nd (0-15) | - | - | Mute Ch 5-9 (0,127) | - | - |
| 13-16 | - | - | - | - | - | Mute Ch 5-9 (0,127) | - | - |
| 17-21 | - | - | - | - | - | Solo Ch 5-9 (0,127) | - | - |
| 22-32 | - | - | - | - | - | Mute/Solo 10-20 (0,127) | - | - |
| 33 | - | - | - | - | - | Sync Mode (0,1,2,3) | - | - |
| 64 | - | - | - | - | Live Mode (0,127) | Perf Mode (0,127) | - | Instance (0-15) |
| 65 | - | - | - | - | - | - | - | Chain Pat (0-127) |
| 66 | - | - | - | - | - | - | - | Sync Mode (0,1,2,3) |
| 67 | - | - | - | - | - | - | - | Auto-Save (0,1) |
| 68 | - | - | - | - | - | - | - | Bypass (0,1) |
| 69 | - | - | - | - | - | - | - | Mix Level (0-127) |
| 70 | - | - | - | - | - | - | - | Lo-Fi (0-127) |
| 71 | - | - | - | - | - | - | - | Stereo Width (0-127) |

### CC Command Legend
- **P Bend**: Pitch Bend (split across 2 CCs for 0-254 range)
- **E Env**: Envelope (1st=amplitude, 2nd=release)
- **S Sweep**: Sweep (Pulse 1 only, 1st=time, 2nd=pitch)
- **S Shape**: Shape (Noise only, 1st=offset, 2nd=loop)
- **O Output**: Pan (0=none, 1=right, 2=left, 3=both)
- **V Vibrato**: Vibrato (1st=speed, 2nd=depth)
- **F Frame**: Wave Frame (1st=direction, 2nd=step)
- **F Tune**: Fine-tune (Pulse 2 only, 1st=TSP, 2nd=steps)
- **W Wave**: Wave synth (1st=speed, 2nd=length)
- **M Volume**: Master Volume
- **T Tempo**: Tempo (global only)
- **G Groove**: Groove template selection

### Program Change Support
- **All Channels**: Program Change 0-15 maps to LSDj patch/instrument slots (0-F)
- **Voice Stealing**: Polyphonic sequences use voice stealing when patch changes occur
- **Per-Channel**: Each MIDI channel has independent patch selection

## MIDI Implementation Strategies

### Pitch Bend Handling
LSDj's P command uses values 0-254 (00-FE hex), which exceeds MIDI CC's 0-127 range. Three implementation options:

#### Option 1: Split Across Two CCs (Recommended)
- **1st CC**: Coarse control (0-15) - represents first hex digit
- **2nd CC**: Fine control (0-14) - represents second hex digit (max FE = 15*16 + 14 = 254)
- **Calculation**: Final value = (1st CC × 16) + 2nd CC
- **Benefits**: Full precision, intuitive hex mapping

#### Option 2: Use MIDI Pitch Bend Messages
- **Standard 14-bit pitch bend wheel**: ±8192 range
- **Mapping**: Center (8192) = P00 (stop), +8192 = PFE (fastest up), -8192 = P01 (fastest down)
- **Benefits**: More natural for keyboard players, standard MIDI

#### Option 3: Scale to 0-127 (Alternative)
- **Mapping**: CC value × 2 = P command value (loses odd values)
- **Benefits**: Single CC control
- **Drawbacks**: 50% precision loss

### Tempo Control Strategy
Tempo commands are **global only** - removed from individual channels to match LSDj behavior:

#### Global Tempo Sources
- **Channel 5 (Master Mode)**: Primary tempo control via T command
- **Channel 7 (Polyphonic Mode)**: Global tempo affecting all channels simultaneously
- **MIDI Clock Sync**: Standard MIDI timing for DAW synchronization

#### Tempo Range Mapping
- **LSDj Range**: T00-T27 (256-295 BPM), T28-TFF (40-255 BPM)
- **MIDI Implementation**: 40-295 BPM continuous range
- **Host Sync**: MIDI Clock messages for precise DAW timing

## SysEx Implementation

### Project Sharing
**Format**: F0 69 69 4C [command] [data...] F7

**Commands**:
- **20**: Send complete song
- **21**: Receive complete song  
- **40**: Send instrument
- **41**: Receive instrument
- **60**: Send chain
- **61**: Receive chain

**Example - Send Instrument**:
```
F0 69 69 4C 40 [instrument_number] [instrument_data] F7
```

### File Operations
**Save Project**: CC 70 with value 127 triggers project save
**Load Project**: CC 71 with value 127 triggers project load
**Clear Project**: CC 72 with value 127 clears current project

## Hardware Compatibility Notes

### LSDj Command Mapping

**Pulse Channel 1**: Supports all synthesis commands including sweep (S command)
**Pulse Channel 2**: Supports all synthesis commands except sweep, includes detune (D command)
**Wave Channel**: Supports wavetable synthesis (SYNTH), sample playback (KIT), and speech synthesis (SP256)
**Noise Channel**: Supports percussion and noise effects with 16 frequency settings

**Removed Features**: The following features have been removed as they do not correspond to actual LSDj commands:
- Sustain pedal (no LSDj equivalent) 
- Vibrato rate/depth separation (LSDj V command combines both)
- Wave channel instrument switching (handled via program change)
- Noise channel controls (reserved for future implementation)

**Voice Stealing**: Polyphonic mode (Channel 7) implements voice stealing for programmed sequences when patch changes occur during playback.

## DAW Integration

### Supported Formats
- **VST3**: Windows and macOS
- **Audio Unit (AU)**: macOS only
- **Standalone**: Cross-platform application

### Recommended DAWs
- **Ableton Live**: Excellent integration with Live mode features
- **Logic Pro**: Full AU support with automation
- **Reaper**: Flexible MIDI routing capabilities
- **FL Studio**: Pattern-based workflow compatibility

### Setup Tips
1. Set MIDI input to 6 channels (1-6)
2. Configure low-latency audio drivers (ASIO/CoreAudio)
3. Use 44.1kHz sample rate for authentic Game Boy sound
4. Enable MIDI thru for live monitoring

## Project Sharing via MIDI

LSDj supports sharing projects and instruments through MIDI SysEx messages:

### Sending Data
**Complete Projects**: Use CC 70 (value 127) to trigger project send
**Individual Instruments**: Send via SysEx F0 69 69 4C 40 [instrument#] [data] F7
**Chains**: Send via SysEx F0 69 69 4C 60 [chain#] [data] F7

### Receiving Data
Projects automatically receive compatible SysEx data when connected to another LSDj instance or compatible device.

### Collaboration Workflow
1. Connect multiple Game Boys via MIDI splitter
2. Share instruments between sessions
3. Transfer complete projects for collaboration
4. Backup projects to DAW via SysEx recording

## Performance Tips

### Low Latency Setup
- Use dedicated audio interface with ASIO drivers
- Set buffer size to 64-128 samples
- Disable unnecessary system services
- Use wired MIDI connections when possible

### Live Performance
- Use Channel 6 for live mode controls
- Map transport controls to hardware controller
- Practice with mute/solo functions for live mixing
- Set up template projects with your preferred CC mappings

### Programming Workflow
1. Start with basic patterns on Channels 1-4
2. Use Global controls (Channel 5) for song structure
3. Add expression with vibrato and envelope CCs
4. Fine-tune with channel-specific synthesis parameters

## References

- [LSDj Official Manual](http://www.littlesounddj.com/lsd/latest/documentation/)
- [MIDI 1.0 Specification](https://www.midi.org/specifications/midi1-specifications)
- [Arduinoboy Interface](https://github.com/trash80/Arduinoboy)
- [RetroPlug VST/AU](https://github.com/tommitytom/RetroPlug)
- [Game Boy Sound Hardware](https://gbdev.gg8.se/wiki/articles/Gameboy_sound_hardware)

## Glossary

**LSDj**: Little Sound DJ - Game Boy music tracker
**Arduinoboy**: MIDI interface hardware for Game Boy
**Chain**: Sequence of 16-step phrases forming song sections  
**Phrase**: 16-step pattern with notes and commands
**Instrument**: Sound preset with synthesis parameters
**Table**: Automation sequence for parameter changes
**Command**: Effect or parameter change instruction (C, H, K, etc.)
**Groove**: Timing template affecting note placement and swing
