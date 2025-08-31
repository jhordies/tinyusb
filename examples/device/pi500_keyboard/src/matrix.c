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

#include "matrix.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "class/hid/hid.h"

// Pi 500 keyboard matrix pin definitions
const uint8_t row_pins[MATRIX_ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
const uint8_t col_pins[MATRIX_COLS] = {27, 8, 9, 10, 11, 12, 13, 14, 23, 24, 22, 20, 29, 18, 15, 21, 28, 26};

// Matrix state
static bool matrix_state[MATRIX_ROWS][MATRIX_COLS];
static bool matrix_prev_state[MATRIX_ROWS][MATRIX_COLS];

// USB HID keycode mapping for Pi 500 keyboard layout
// Based on QMK keyboard.json layout coordinates
const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
  // Row 0: GP0
  {0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, 0, HID_KEY_PAUSE_BREAK, 0, 0, 0, 0, 0, 0, 0},
  
  // Row 1: GP1  
  {0, HID_KEY_TAB, HID_KEY_CAPS_LOCK, HID_KEY_F3, HID_KEY_T, HID_KEY_Y, HID_KEY_BRACKET_LEFT, HID_KEY_F7, HID_KEY_BRACKET_RIGHT, 0, HID_KEY_BACKSPACE, 0, 0, 0, 0, HID_KEY_SHIFT_LEFT, HID_KEY_ALT_LEFT, 0},
  
  // Row 2: GP2
  {0, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_GUI_LEFT, HID_KEY_BACKSLASH, 0, 0, 0, 0, HID_KEY_SHIFT_RIGHT, 0, 0},
  
  // Row 3: GP3
  {0, HID_KEY_ESCAPE, HID_KEY_PRINT_SCREEN, HID_KEY_F4, HID_KEY_G, HID_KEY_H, HID_KEY_F6, 0, HID_KEY_APOSTROPHE, 0, 0, HID_KEY_SPACE, 0, 0, HID_KEY_ARROW_UP, 0, 0, 0},
  
  // Row 4: GP4
  {HID_KEY_CONTROL_RIGHT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SCROLL_LOCK, 0, HID_KEY_ENTER, HID_KEY_F11, 0, 0, 0, 0, 0, 0},
  
  // Row 5: GP5
  {0, HID_KEY_HOME, HID_KEY_PAGE_UP, HID_KEY_PAGE_DOWN, HID_KEY_B, 0, HID_KEY_INSERT, HID_KEY_END, HID_KEY_SLASH, HID_KEY_ALT_RIGHT, 0, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT, 0, HID_KEY_ARROW_LEFT, 0, 0, 0},
  
  // Row 6: GP6
  {HID_KEY_CONTROL_LEFT, HID_KEY_GRAVE, HID_KEY_F1, HID_KEY_F2, HID_KEY_5, HID_KEY_6, HID_KEY_EQUAL, HID_KEY_F8, HID_KEY_MINUS, 0, HID_KEY_F9, HID_KEY_DELETE, 0, 0, 0, 0, 0, 0},
  
  // Row 7: GP7
  {HID_KEY_F5, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_F12, HID_KEY_F10, 0, 0, 0, 0, 0, 0, 0}
};

void matrix_init(void) {
  // Initialize row pins as inputs with pull-up
  for (int i = 0; i < MATRIX_ROWS; i++) {
    gpio_init(row_pins[i]);
    gpio_set_dir(row_pins[i], GPIO_IN);
    gpio_pull_up(row_pins[i]);
  }
  
  // Initialize column pins as outputs, set high
  for (int i = 0; i < MATRIX_COLS; i++) {
    gpio_init(col_pins[i]);
    gpio_set_dir(col_pins[i], GPIO_OUT);
    gpio_put(col_pins[i], 1);
  }
  
  // Initialize matrix state
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      matrix_state[row][col] = false;
      matrix_prev_state[row][col] = false;
    }
  }
}

void matrix_scan(void) {
  // Save previous state
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      matrix_prev_state[row][col] = matrix_state[row][col];
    }
  }
  
  // Scan matrix (ROW2COL)
  for (int col = 0; col < MATRIX_COLS; col++) {
    // Set current column low
    gpio_put(col_pins[col], 0);
    sleep_us(1); // Small delay for signal to settle
    
    // Read all rows
    for (int row = 0; row < MATRIX_ROWS; row++) {
      // Key is pressed if row reads low (pulled down by column)
      matrix_state[row][col] = !gpio_get(row_pins[row]);
    }
    
    // Set column back to high
    gpio_put(col_pins[col], 1);
  }
}

bool matrix_is_key_pressed(uint8_t row, uint8_t col) {
  if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
  return matrix_state[row][col];
}

uint8_t matrix_get_keycode(uint8_t row, uint8_t col) {
  if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return 0;
  return keymap[row][col];
}