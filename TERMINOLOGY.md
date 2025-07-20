# Official Terminology Reference

This document clarifies the terminology used in DUYinoboy and aligns it with official documentation sources.

## Official Source Alignment

DUYinoboy uses terminology that matches the official documentation from:

### LSDJ (Little Sound DJ)
- **Official Site**: [https://www.littlesounddj.com/](https://www.littlesounddj.com/)
- **Sync Modes**: "Master" and "Slave" (official LSDJ terms)
- **Keyboard Mode**: "Keyboard" mode for PC keyboard interface

### ArduinoBoy
- **Official Repository**: [https://github.com/trash80/Arduinoboy](https://github.com/trash80/Arduinoboy)
- **Mode Names**: 
  - Mode 1: "LSDJ as MIDI Slave Sync"
  - Mode 2: "LSDJ as MIDI Master Sync"
  - Mode 3: "LSDJ PC Keyboard mode"
  - Mode 4: "MIDI to Nanoloop sync"
  - Mode 6: "LSDJ MIDIMAP"
  - Mode 7: "LSDJ MIDIOUT"

### Retrokits RK-002
- **Official Site**: [https://retrokits.com/shop/rk002/](https://retrokits.com/shop/rk002/)
- **Hardware**: MIDI input only (no MIDI output capability)
- **Communication**: GPIO pins for Game Boy interface

## Important Clarifications

### Master/Slave vs Clock Generation
- **"Master" modes**: Generate timing clocks via GPIO pins to Game Boy
- **"Slave" modes**: Follow incoming MIDI clock signals
- **NOT MIDI Output**: RK-002 cannot send MIDI messages out
- **GPIO Communication**: All Game Boy communication via 2 GPIO pins

### Compatibility Notes
1. **LSDJ MIDIOUT Mode**: Not supported on RK-002 (requires MIDI output)
2. **Master Sync**: Creates GPIO clock signals, not MIDI clock output
3. **Slave Sync**: Receives MIDI clock, converts to GPIO signals

## Mode Mapping

| Mode | Official ArduinoBoy Name | RK-002 Compatible | Notes |
|------|--------------------------|-------------------|-------|
| 0 | LSDJ as MIDI Master Sync | ✅ Yes | GPIO clock generation |
| 1 | LSDJ as MIDI Slave Sync | ✅ Yes | MIDI clock → GPIO |
| 2 | LSDJ PC Keyboard mode | ✅ Yes | MIDI notes → GPIO data |
| 3 | LSDJ MIDIMAP | ✅ Yes | 4-channel polyphonic |
| 4 | MIDI to Nanoloop sync | ✅ Yes | Master mode |
| 5 | MIDI to Nanoloop sync | ✅ Yes | Slave mode |
| 6 | Custom Mode 1 | ✅ Yes | User-definable |
| 7 | LSDJ MIDIOUT | ❌ No | Requires MIDI output |

## References

- [LSDJ Multiboot MIDI Sync](https://littlesounddj.fandom.com/wiki/Multiboot_MIDI_Sync)
- [ArduinoBoy GitHub Repository](https://github.com/trash80/Arduinoboy)
- [RK-002 Smart MIDI Cable](https://retrokits.com/shop/rk002/)
- [DUY Web Editor](https://duy.retrokits.com/)
