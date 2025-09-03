/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif

// --- LED CONFIGURATION ---
#define LED_PIN               17  // Pi 500 LED on GP17
#define LED_STATE_ON          1

// --- BUTTON CONFIGURATION ---
// Pi 500 doesn't have a user button on the RP2040 board
#define BUTTON_PIN            -1
#define BUTTON_STATE_ACTIVE   0

// --- UART CONFIGURATION ---
#define UART_DEV              0
#define UART_TX_PIN           16  // Pi 500 debug UART TX pin
#define UART_RX_PIN           1   // Standard RX pin

// --- PI 500 KEYBOARD MATRIX ---
// Matrix dimensions: 8 rows x 18 columns
// Row pins: GP0-GP7
// Col pins: GP27,GP8-GP15,GP18,GP20-GP24,GP26-GP29
// Scanning: ROW2COL, no diodes (ghost keys possible)

// --- PI 500 SYSTEM PINS ---
// All GPIO pins are reserved for Pi 500 hardware functions:
// GP0-GP7: Keyboard matrix rows
// GP8-GP15: Keyboard matrix columns (partial)
// GP16: Debug UART TX
// GP17: LED (heartbeat/debug)
// GP18: Keyboard matrix column
// GP19: Power button control (PWR_BTN) - CRITICAL for Pi power management
// GP20: Power key detection column (separate from matrix)
// GP21-GP24: Keyboard matrix columns
// GP25: Caps Lock LED
// GP26-GP29: Keyboard matrix columns
// Available for user applications: NONE

// ⚠️  POWER MANAGEMENT WARNING:
// Custom firmware MUST implement power button handling via GP19 or the Pi 500
// power button will stop working, potentially making the device unbootable.

#ifdef __cplusplus
 }
#endif

#endif