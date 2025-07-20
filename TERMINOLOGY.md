# Official Terminology Reference

This document clarifies the terminology used in DUYinoboy and aligns it with official documentation sources.

**Important**: DUYinoboy is designed for MIDI input only - LSDJ and Nanoloop run as MIDI clock followers.

## Official Source Alignment

DUYinoboy uses terminology that matches the official documentation from:

### LSDJ (Little Sound DJ)
- **Official Site**: [https://www.littlesounddj.com/](https://www.littlesounddj.com/)
- **Sync Mode**: "Slave" mode only (follows external MIDI clock)
- **Keyboard Mode**: "Keyboard" mode for PC keyboard interface

### ArduinoBoy
- **Official Repository**: [https://github.com/trash80/Arduinoboy](https://github.com/trash80/Arduinoboy)
- **Adapted Modes**: 
  - LSDJ MIDI Clock Sync (based on "LSDJ as MIDI Slave Sync")
  - LSDJ PC Keyboard mode
  - LSDJ MIDIMAP
  - Nanoloop MIDI Clock Sync (based on "MIDI to Nanoloop sync")

### Retrokits RK-002
- **Official Site**: [https://retrokits.com/shop/rk002/](https://retrokits.com/shop/rk002/)
- **Hardware**: MIDI input only (no MIDI output capability)
- **Communication**: GPIO pins for Game Boy interface

## Important Clarifications

### MIDI Clock Follower Only
- **Clock Sync**: Follows incoming MIDI clock signals
- **No Master Mode**: Cannot generate internal clocks (RK-002 limitation)
- **External Clock Required**: DAW/sequencer must provide MIDI clock
- **GPIO Communication**: All Game Boy communication via 2 GPIO pins

### Compatibility Notes
1. **No Master Modes**: RK-002 cannot generate MIDI clock (MIDI input only)
2. **Clock Follower**: All timing comes from external MIDI clock source
3. **GPIO Conversion**: MIDI clock → GPIO timing signals for Game Boy

## Mode Mapping

| Mode | DUYinoboy Function | RK-002 Compatible | Notes |
|------|-------------------|-------------------|-------|
| 0 | LSDJ MIDI Clock Sync | ✅ Yes | MIDI clock → GPIO timing |
| 1 | LSDJ PC Keyboard mode | ✅ Yes | MIDI notes → GPIO data |
| 2 | LSDJ MIDIMAP | ✅ Yes | 4-channel polyphonic |
| 3 | Nanoloop MIDI Clock Sync | ✅ Yes | MIDI clock → GPIO timing |
| 4-7 | Custom Modes | ✅ Yes | User-definable |

## References

- [LSDJ Multiboot MIDI Sync](https://littlesounddj.fandom.com/wiki/Multiboot_MIDI_Sync)
- [ArduinoBoy GitHub Repository](https://github.com/trash80/Arduinoboy)
- [RK-002 Smart MIDI Cable](https://retrokits.com/shop/rk002/)
- [DUY Web Editor](https://duy.retrokits.com/)
