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

#include "dual_mode.h"
#include "matrix.h"
#include "tusb.h"
#include "class/hid/hid.h"
#include "bsp/board_api.h"

// Current operating mode
static device_mode_t current_mode = MODE_KEYBOARD;

// Key state tracking
static bool key_states[MATRIX_ROWS][MATRIX_COLS];
static bool prev_key_states[MATRIX_ROWS][MATRIX_COLS];

// Mode switch detection
static bool mode_switch_pressed = false;
static uint32_t mode_switch_time = 0;

// Keyboard report
static uint8_t keyboard_report[6] = {0};
static uint8_t keyboard_modifier = 0;

// MIDI key mapping (same as MIDI keyboard example)
typedef struct {
  uint8_t row, col, midi_note;
} key_midi_map_t;

static const key_midi_map_t white_keys[] = {
  {1, 1, MIDI_NOTE_C4}, {0, 1, MIDI_NOTE_C4 + 2}, {0, 2, MIDI_NOTE_C4 + 4}, {0, 3, MIDI_NOTE_C4 + 5},
  {0, 4, MIDI_NOTE_C4 + 7}, {1, 4, MIDI_NOTE_C4 + 9}, {1, 5, MIDI_NOTE_C4 + 11}, {0, 5, MIDI_NOTE_C4 + 12},
  {0, 6, MIDI_NOTE_C4 + 14}, {0, 7, MIDI_NOTE_C4 + 16}, {0, 8, MIDI_NOTE_C4 + 17}, {1, 8, MIDI_NOTE_C4 + 19},
  {1, 6, MIDI_NOTE_C4 + 21}, {0, 0, 0}
};

static const key_midi_map_t black_keys[] = {
  {7, 1, MIDI_NOTE_C4 + 1}, {7, 2, MIDI_NOTE_C4 + 3}, {7, 4, MIDI_NOTE_C4 + 6}, {6, 4, MIDI_NOTE_C4 + 8},
  {6, 5, MIDI_NOTE_C4 + 10}, {7, 6, MIDI_NOTE_C4 + 13}, {7, 7, MIDI_NOTE_C4 + 15}, {6, 8, MIDI_NOTE_C4 + 18},
  {6, 6, MIDI_NOTE_C4 + 20}, {0, 0, 0}
};

// Keyboard keymap (simplified)
static const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
  {0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, 0, HID_KEY_PAUSE_BREAK, 0, 0, 0, 0, 0, 0, 0},
  {0, HID_KEY_TAB, HID_KEY_CAPS_LOCK, HID_KEY_F3, HID_KEY_T, HID_KEY_Y, HID_KEY_BRACKET_LEFT, HID_KEY_F7, HID_KEY_BRACKET_RIGHT, 0, HID_KEY_BACKSPACE, 0, 0, 0, 0, HID_KEY_SHIFT_LEFT, HID_KEY_ALT_LEFT, 0},
  {0, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_GUI_LEFT, HID_KEY_BACKSLASH, 0, 0, 0, 0, HID_KEY_SHIFT_RIGHT, 0, 0},
  {0, HID_KEY_ESCAPE, HID_KEY_PRINT_SCREEN, HID_KEY_F4, HID_KEY_G, HID_KEY_H, HID_KEY_F6, 0, HID_KEY_APOSTROPHE, 0, 0, HID_KEY_SPACE, 0, 0, HID_KEY_ARROW_UP, 0, 0, 0},
  {HID_KEY_CONTROL_RIGHT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SCROLL_LOCK, 0, HID_KEY_ENTER, HID_KEY_F11, 0, 0, 0, 0, 0, 0},
  {0, HID_KEY_HOME, HID_KEY_PAGE_UP, HID_KEY_PAGE_DOWN, HID_KEY_B, 0, HID_KEY_INSERT, HID_KEY_END, HID_KEY_SLASH, HID_KEY_ALT_RIGHT, 0, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT, 0, HID_KEY_ARROW_LEFT, 0, 0, 0},
  {HID_KEY_CONTROL_LEFT, HID_KEY_GRAVE, HID_KEY_F1, HID_KEY_F2, HID_KEY_5, HID_KEY_6, HID_KEY_EQUAL, HID_KEY_F8, HID_KEY_MINUS, 0, HID_KEY_F9, HID_KEY_DELETE, 0, 0, 0, 0, 0, 0},
  {HID_KEY_F5, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_F12, HID_KEY_F10, 0, 0, 0, 0, 0, 0, 0}
};

void dual_mode_init(void) {
  current_mode = MODE_KEYBOARD;
  
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      key_states[row][col] = false;
      prev_key_states[row][col] = false;
    }
  }
}

device_mode_t get_current_mode(void) {
  return current_mode;
}

void check_mode_switch(void) {
  bool fn_pressed = matrix_is_key_pressed(MODE_SWITCH_ROW1, MODE_SWITCH_COL1);
  bool scroll_pressed = matrix_is_key_pressed(MODE_SWITCH_ROW2, MODE_SWITCH_COL2);
  
  if (fn_pressed && scroll_pressed) {
    if (!mode_switch_pressed) {
      mode_switch_pressed = true;
      mode_switch_time = board_millis();
    } else if (board_millis() - mode_switch_time > 500) { // 500ms hold
      current_mode = (current_mode == MODE_KEYBOARD) ? MODE_MIDI : MODE_KEYBOARD;
      mode_switch_time = board_millis() + 1000; // Prevent rapid switching
      
      // Clear all key states when switching modes
      for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
          key_states[row][col] = false;
          prev_key_states[row][col] = false;
        }
      }
    }
  } else {
    mode_switch_pressed = false;
  }
}

uint8_t get_midi_note_for_key(uint8_t row, uint8_t col) {
  for (int i = 0; white_keys[i].midi_note != 0; i++) {
    if (white_keys[i].row == row && white_keys[i].col == col) {
      return white_keys[i].midi_note;
    }
  }
  
  for (int i = 0; black_keys[i].midi_note != 0; i++) {
    if (black_keys[i].row == row && black_keys[i].col == col) {
      return black_keys[i].midi_note;
    }
  }
  
  return 0;
}

void send_note_on(uint8_t note, uint8_t velocity) {
  uint8_t note_on[3] = {0x90, note, velocity};
  tud_midi_stream_write(0, note_on, 3);
}

void send_note_off(uint8_t note) {
  uint8_t note_off[3] = {0x80, note, 0};
  tud_midi_stream_write(0, note_off, 3);
}

void process_midi_mode(void) {
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      // Skip mode switch keys
      if ((row == MODE_SWITCH_ROW1 && col == MODE_SWITCH_COL1) ||
          (row == MODE_SWITCH_ROW2 && col == MODE_SWITCH_COL2)) continue;
      
      if (key_states[row][col] && !prev_key_states[row][col]) {
        uint8_t midi_note = get_midi_note_for_key(row, col);
        if (midi_note > 0) {
          send_note_on(midi_note, MIDI_VELOCITY);
        }
      } else if (!key_states[row][col] && prev_key_states[row][col]) {
        uint8_t midi_note = get_midi_note_for_key(row, col);
        if (midi_note > 0) {
          send_note_off(midi_note);
        }
      }
    }
  }
}

void send_keyboard_report(void) {
  if (!tud_hid_ready()) return;
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboard_modifier, keyboard_report);
}

void process_keyboard_mode(void) {
  keyboard_modifier = 0;
  memset(keyboard_report, 0, sizeof(keyboard_report));
  int report_index = 0;
  
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      // Skip mode switch keys
      if ((row == MODE_SWITCH_ROW1 && col == MODE_SWITCH_COL1) ||
          (row == MODE_SWITCH_ROW2 && col == MODE_SWITCH_COL2)) continue;
      
      if (key_states[row][col]) {
        uint8_t keycode = keymap[row][col];
        if (keycode >= HID_KEY_CONTROL_LEFT && keycode <= HID_KEY_GUI_RIGHT) {
          // Modifier key
          keyboard_modifier |= (1 << (keycode - HID_KEY_CONTROL_LEFT));
        } else if (keycode > 0 && report_index < 6) {
          // Regular key
          keyboard_report[report_index++] = keycode;
        }
      }
    }
  }
  
  send_keyboard_report();
}

void dual_mode_task(void) {
  matrix_scan();
  
  // Update key states
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      prev_key_states[row][col] = key_states[row][col];
      key_states[row][col] = matrix_is_key_pressed(row, col);
    }
  }
  
  check_mode_switch();
  
  if (current_mode == MODE_KEYBOARD) {
    process_keyboard_mode();
  } else {
    process_midi_mode();
  }
}