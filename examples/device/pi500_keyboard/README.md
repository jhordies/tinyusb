# Pi 500 Keyboard Example

This example demonstrates how to implement a USB HID keyboard using TinyUSB on the Raspberry Pi 500's embedded RP2350 microcontroller.

## Overview

The Pi 500 contains an RP2350 chip that handles keyboard input via an 8×18 matrix connected through an FFC connector. This example replicates the original keyboard functionality using TinyUSB instead of QMK.

## Hardware

- **Microcontroller**: RP2350A (ARM Cortex-M33 + RISC-V Hazard3)
- **Matrix**: 8 rows × 18 columns, ROW2COL scanning
- **Flash**: W25X10CL (DSPI mode)
- **USB**: Connected to Pi 500 USB lines

### Pin Configuration

#### Matrix Rows (8 pins)
- GP0-GP7: Keyboard matrix rows

#### Matrix Columns (18 pins)  
- GP27, GP8-GP15, GP18, GP20-GP24, GP26-GP29: Keyboard matrix columns

#### Special Pins
- GP16: Debug UART TX
- GP25: LED

## Features

- Full Pi 500 keyboard matrix scanning
- USB HID keyboard interface
- Proper key mapping matching physical layout
- Remote wakeup support
- Caps Lock LED indication

## Building

Requires Pi 500 board support in both Pico SDK and TinyUSB:

```bash
mkdir build && cd build
cmake -DFAMILY=rp2040 -DBOARD=raspberry_pi_pi500 ..
make
```

## Flashing

1. Put Pi 500 RP2350 into BOOTSEL mode
2. Copy the generated UF2 file to the RPI-RP2 drive
3. Device will reboot and enumerate as "Pi 500 Keyboard"

## Usage

Once flashed, the Pi 500 keyboard will function as a standard USB HID keyboard. All keys are mapped according to their physical positions on the Pi 500 keyboard layout.

## Key Mapping

The keymap follows the Pi 500 physical layout with matrix coordinates matching the QMK implementation:

- Function row: ESC, F1-F12, DEL
- Number row: `, 1-9, 0, -, =, Backspace  
- QWERTY row: Tab, Q-P, [, ], \
- ASDFGH row: Caps, A-L, ;, ', Enter
- ZXCVBN row: LShift, Z-M, ,, ., /, RShift
- Bottom row: LCtrl, LWin, LAlt, Fn, Space, RAlt, RCtrl
- Arrow cluster: Up, Down, Left, Right
- Function keys: PrtSc, ScrLk, Pause, Ins, Home, PgUp, PgDn, End

## Notes

- Matrix scanning uses ROW2COL direction
- No diodes in matrix (ghost keys possible with certain combinations)
- Supports remote wakeup for Pi 500 wake-on-keyboard
- Compatible with existing Pi 500 hardware
- Can be reflashed back to original QMK keyboard firmware