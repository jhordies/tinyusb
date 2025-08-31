/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Raspberry Pi Ltd
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
 */

#ifndef DUAL_MODE_H_
#define DUAL_MODE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Operating modes
typedef enum {
  MODE_KEYBOARD = 0,
  MODE_MIDI = 1
} device_mode_t;

// Mode switching key combination: Fn + Print Screen
#define MODE_SWITCH_ROW1 3  // Fn key
#define MODE_SWITCH_COL1 9
#define MODE_SWITCH_ROW2 3  // Print Screen key  
#define MODE_SWITCH_COL2 2

// MIDI note definitions
#define MIDI_NOTE_C4  60
#define MIDI_VELOCITY 64

// Dual mode functions
void dual_mode_init(void);
void dual_mode_task(void);
device_mode_t get_current_mode(void);
void check_mode_switch(void);

// Keyboard functions
void process_keyboard_mode(void);
void send_keyboard_report(void);

// MIDI functions  
void process_midi_mode(void);
uint8_t get_midi_note_for_key(uint8_t row, uint8_t col);
void send_note_on(uint8_t note, uint8_t velocity);
void send_note_off(uint8_t note);

#ifdef __cplusplus
}
#endif

#endif /* DUAL_MODE_H_ */