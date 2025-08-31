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
#define LED_PIN               25
#define LED_STATE_ON          1

// --- BUTTON CONFIGURATION ---
// Pi 500 doesn't have a user button on the RP2350 board
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

// --- PI 500 RESERVED PINS ---
// The following pins are used by Pi 500 keyboard matrix:
// GP0,GP1,GP2,GP3,GP4,GP5,GP6,GP7 (rows)
// GP8,GP9,GP10,GP11,GP12,GP13,GP14,GP15 (cols)
// GP18,GP20,GP21,GP22,GP23,GP24 (cols)
// GP26,GP27,GP28,GP29 (cols)
// GP16 (debug UART TX)
// Available for user applications: GP17,GP19,GP25

#ifdef __cplusplus
 }
#endif

#endif