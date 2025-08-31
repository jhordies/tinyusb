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

// Pi 500 keyboard matrix pin definitions
const uint8_t row_pins[MATRIX_ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
const uint8_t col_pins[MATRIX_COLS] = {27, 8, 9, 10, 11, 12, 13, 14, 23, 24, 22, 20, 29, 18, 15, 21, 28, 26};

// Matrix state
static bool matrix_state[MATRIX_ROWS][MATRIX_COLS];

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
    }
  }
}

void matrix_scan(void) {
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