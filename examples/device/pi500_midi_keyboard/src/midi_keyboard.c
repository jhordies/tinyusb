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

#include "midi_keyboard.h"
#include "matrix.h"
#include "tusb.h"

// MIDI note mapping for Pi 500 keyboard
// White keys: TAB=C, Q=D, W=E, E=F, R=G, T=A, Y=B, U=C+1, I=D+1, O=E+1, P=F+1, [=G+1, ]=A+1
// Black keys: 1=C#, 2=D#, (3 skip), 4=F#, 5=G#, 6=A#, (7 skip), 8=C#+1, 9=D#+1, (0 skip), -=F#+1, ==G#+1
static const key_midi_map_t white_keys[] = {
  {1, 1, MIDI_NOTE_C4},      // TAB = C4
  {0, 1, MIDI_NOTE_C4 + 2},  // Q = D4
  {0, 2, MIDI_NOTE_C4 + 4},  // W = E4
  {0, 3, MIDI_NOTE_C4 + 5},  // E = F4
  {0, 4, MIDI_NOTE_C4 + 7},  // R = G4
  {1, 4, MIDI_NOTE_C4 + 9},  // T = A4
  {1, 5, MIDI_NOTE_C4 + 11}, // Y = B4
  {0, 5, MIDI_NOTE_C4 + 12}, // U = C5
  {0, 6, MIDI_NOTE_C4 + 14}, // I = D5
  {0, 7, MIDI_NOTE_C4 + 16}, // O = E5
  {0, 8, MIDI_NOTE_C4 + 17}, // P = F5
  {1, 8, MIDI_NOTE_C4 + 19}, // [ = G5
  {1, 6, MIDI_NOTE_C4 + 21}, // ] = A5
  {0, 0, 0} // End marker
};

static const key_midi_map_t black_keys[] = {
  {7, 1, MIDI_NOTE_C4 + 1},  // 1 = C#4
  {7, 2, MIDI_NOTE_C4 + 3},  // 2 = D#4
  // 3 is skipped (no black key between E and F)
  {7, 4, MIDI_NOTE_C4 + 6},  // 4 = F#4
  {6, 4, MIDI_NOTE_C4 + 8},  // 5 = G#4
  {6, 5, MIDI_NOTE_C4 + 10}, // 6 = A#4
  // 7 is skipped (no black key between B and C)
  {7, 6, MIDI_NOTE_C4 + 13}, // 8 = C#5
  {7, 7, MIDI_NOTE_C4 + 15}, // 9 = D#5
  // 0 is skipped (no black key between E and F)
  {6, 8, MIDI_NOTE_C4 + 18}, // - = F#5
  {6, 6, MIDI_NOTE_C4 + 20}, // = = G#5
  {0, 0, 0} // End marker
};

// Key state tracking
static bool key_states[MATRIX_ROWS][MATRIX_COLS];
static bool prev_key_states[MATRIX_ROWS][MATRIX_COLS];

void midi_keyboard_init(void) {
  // Initialize key states
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      key_states[row][col] = false;
      prev_key_states[row][col] = false;
    }
  }
}

uint8_t get_midi_note_for_key(uint8_t row, uint8_t col) {
  // Check white keys
  for (int i = 0; white_keys[i].midi_note != 0; i++) {
    if (white_keys[i].row == row && white_keys[i].col == col) {
      return white_keys[i].midi_note;
    }
  }
  
  // Check black keys
  for (int i = 0; black_keys[i].midi_note != 0; i++) {
    if (black_keys[i].row == row && black_keys[i].col == col) {
      return black_keys[i].midi_note;
    }
  }
  
  return 0; // No MIDI note for this key
}

void send_note_on(uint8_t note, uint8_t velocity) {
  uint8_t note_on[3] = {0x90, note, velocity}; // Note On, Channel 1
  tud_midi_stream_write(0, note_on, 3);
}

void send_note_off(uint8_t note) {
  uint8_t note_off[3] = {0x80, note, 0}; // Note Off, Channel 1
  tud_midi_stream_write(0, note_off, 3);
}

void midi_keyboard_task(void) {
  // Scan matrix
  matrix_scan();
  
  // Update key states
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      prev_key_states[row][col] = key_states[row][col];
      key_states[row][col] = matrix_is_key_pressed(row, col);
      
      // Check for key press/release events
      if (key_states[row][col] && !prev_key_states[row][col]) {
        // Key pressed
        uint8_t midi_note = get_midi_note_for_key(row, col);
        if (midi_note > 0) {
          send_note_on(midi_note, MIDI_VELOCITY);
        }
      } else if (!key_states[row][col] && prev_key_states[row][col]) {
        // Key released
        uint8_t midi_note = get_midi_note_for_key(row, col);
        if (midi_note > 0) {
          send_note_off(midi_note);
        }
      }
    }
  }
}