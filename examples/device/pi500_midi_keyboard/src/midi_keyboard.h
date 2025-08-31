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

#ifndef MIDI_KEYBOARD_H_
#define MIDI_KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// MIDI note definitions
#define MIDI_NOTE_C4  60  // Middle C
#define MIDI_VELOCITY 64  // Default velocity

// Key to MIDI note mapping
typedef struct {
  uint8_t row;
  uint8_t col;
  uint8_t midi_note;
} key_midi_map_t;

// MIDI keyboard functions
void midi_keyboard_init(void);
void midi_keyboard_task(void);
uint8_t get_midi_note_for_key(uint8_t row, uint8_t col);
void send_note_on(uint8_t note, uint8_t velocity);
void send_note_off(uint8_t note);

#ifdef __cplusplus
}
#endif

#endif /* MIDI_KEYBOARD_H_ */