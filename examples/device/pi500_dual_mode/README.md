# Pi 500 Dual Mode Example

This example combines HID keyboard and MIDI functionality in a single device, allowing runtime switching between keyboard and MIDI piano modes using a simple key combination.

## Overview

The Pi 500 keyboard can operate in two modes:
- **Keyboard Mode**: Standard USB HID keyboard functionality
- **MIDI Mode**: Piano-style MIDI controller with same key mapping as pi500_midi_keyboard

## Mode Switching

**Key Combination**: **Fn + Print Screen** (hold for 500ms)

- **Fn**: [3,9] - Function key (bottom row, clearly labeled)
- **Print Screen**: [3,2] - Print Screen key (numpad area, clearly labeled)

This combination is rarely used in normal typing, making it safe for mode switching.

## Visual Indicators

**LED Blink Patterns**:
- **Keyboard Mode**: Slow blink (1000ms) - steady operation
- **MIDI Mode**: Fast blink (250ms) - musical activity

## Keyboard Mode

Full Pi 500 keyboard functionality with proper key mapping:
- All standard keys (letters, numbers, symbols)
- Function keys (F1-F12)
- Modifiers (Ctrl, Alt, Shift, Win)
- Arrow keys and navigation keys
- Special keys (Print Screen, Pause, etc.)

## MIDI Mode

Piano-style interface using keyboard keys:

### White Keys (QWERTY Row)
```
TAB = C4    Q = D4    W = E4    E = F4    R = G4    T = A4    Y = B4
U = C5      I = D5    O = E5    P = F5    [ = G5    ] = A5
```

### Black Keys (Number Row)
```
1 = C#4    2 = D#4    (3 skip)    4 = F#4    5 = G#4    6 = A#4
(7 skip)   8 = C#5    9 = D#5     (0 skip)   - = F#5    = = G#5
```

## Hardware

- **Microcontroller**: RP2350A on Pi 500
- **Matrix**: 8 rows × 18 columns, ROW2COL scanning
- **USB**: Composite device (HID + MIDI)
- **VID/PID**: 0x2E8A:0x0012 (unique for dual mode)

## Features

- **Runtime mode switching** without reconnection
- **Composite USB device** with both HID and MIDI interfaces
- **State isolation** between modes (no key bleed-through)
- **Debounced mode switching** (500ms hold, 1s cooldown)
- **Visual feedback** via LED blink patterns

## Building

Requires Pi 500 board support:

```bash
mkdir build && cd build
cmake -DFAMILY=rp2040 -DBOARD=raspberry_pi_pi500 ..
make
```

## Flashing

1. Put Pi 500 RP2350 into BOOTSEL mode
2. Copy generated UF2 file to RPI-RP2 drive
3. Device reboots and enumerates as "Pi 500 Dual Mode"

## Usage

### Initial Mode
Device starts in **Keyboard Mode** by default.

### Switching to MIDI Mode
1. Hold **Fn + Print Screen** for 500ms
2. LED changes to fast blink (250ms)
3. Device now sends MIDI messages instead of keyboard input

### Switching Back to Keyboard Mode
1. Hold **Fn + Print Screen** for 500ms again
2. LED changes to slow blink (1000ms)
3. Device returns to normal keyboard operation

### Testing

**Keyboard Mode**:
```bash
# Type normally - should work as standard keyboard
```

**MIDI Mode**:
```bash
# List MIDI devices
amidi -l

# Monitor MIDI messages
amidi -p hw:X,0,0 -d
```

## USB Device Detection

The device enumerates as a composite device with both interfaces:
- **HID Keyboard Interface**: Always present
- **MIDI Interface**: Always present
- **Active Interface**: Determined by current mode

Both interfaces are always available to the host, but only the active mode processes key presses.

## Key Mapping Details

### Mode Switch Keys (Always Reserved)
- **Fn**: [3,9] - Function key
- **Print Screen**: [3,2] - Print Screen key

These keys are excluded from both keyboard and MIDI processing to prevent conflicts.

### Keyboard Mode Keymap
Complete Pi 500 layout with all keys mapped to appropriate HID keycodes.

### MIDI Mode Keymap
Piano layout spanning two octaves (C4-A5) using white and black key positions.

## Notes

- Mode switching requires both keys held simultaneously
- 500ms hold time prevents accidental switching
- 1 second cooldown prevents rapid mode changes
- Key states are cleared when switching modes
- LED provides immediate visual feedback
- Compatible with existing Pi 500 hardware
- Can be reflashed back to original keyboard firmware