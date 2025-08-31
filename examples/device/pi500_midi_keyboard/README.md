# Pi 500 MIDI Keyboard Example

This example turns the Raspberry Pi 500's keyboard into a USB MIDI controller using TinyUSB. Keys are mapped to piano notes with press/release triggering MIDI Note On/Off messages.

## Overview

The Pi 500's 8×18 keyboard matrix is scanned for key presses and releases. Selected keys are mapped to MIDI notes spanning two octaves, creating a piano-like interface using the computer keyboard.

## Key Mapping

### White Keys (QWERTY Row)
Piano layout starting from C4 (Middle C):
```
TAB = C4    Q = D4    W = E4    E = F4    R = G4    T = A4    Y = B4
U = C5      I = D5    O = E5    P = F5    [ = G5    ] = A5
```

### Black Keys (Number Row)  
Sharp/flat notes between white keys:
```
1 = C#4    2 = D#4    (3 skip)    4 = F#4    5 = G#4    6 = A#4
(7 skip)   8 = C#5    9 = D#5     (0 skip)   - = F#5    = = G#5
```

Note: Keys 3, 7, and 0 are skipped as there are no black keys between E-F and B-C in piano layout.

## Hardware

- **Microcontroller**: RP2350A on Pi 500
- **Matrix**: 8 rows × 18 columns, ROW2COL scanning
- **USB**: MIDI device class (VID: 0x2E8A, PID: 0x0011)
- **Pins**: Same GPIO mapping as Pi 500 keyboard

## Features

- Real-time MIDI Note On/Off messages
- Two-octave range (C4-A5)
- Proper piano key layout mapping
- USB MIDI device enumeration
- Low-latency key scanning (1ms polling)

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
3. Device reboots and enumerates as "Pi 500 MIDI Keyboard"

## Usage

### Testing MIDI Output
```bash
# List MIDI devices
amidi -l

# Monitor MIDI messages
amidi -p hw:X,0,0 -d
```

### With DAW/Software
- Connect to any MIDI-compatible software
- Use as piano keyboard input
- Notes span C4 (Middle C) to A5
- Velocity fixed at 64 (medium)

## MIDI Implementation

- **Channel**: 1
- **Note Range**: C4 (60) to A5 (81)
- **Velocity**: 64 (fixed)
- **Messages**: Note On (0x90), Note Off (0x80)
- **Polyphony**: Limited by USB bandwidth and matrix scanning

## Key Layout Diagram

```
Number Row (Black Keys):
[`] [1] [2] [3] [4] [5] [6] [7] [8] [9] [0] [-] [=] [BKSP]
     C#  D#  --  F#  G#  A#  --  C#  D#  --  F#  G#

QWERTY Row (White Keys):
[TAB] [Q] [W] [E] [R] [T] [Y] [U] [I] [O] [P] [[] []] [\]
  C    D   E   F   G   A   B   C   D   E   F   G   A
```

## Notes

- Matrix scanning detects key press/release events
- Ghost keys possible due to no diodes in Pi 500 matrix
- Other keyboard keys (letters, arrows, etc.) are ignored
- Can be reflashed back to original keyboard firmware
- Compatible with standard MIDI software and hardware