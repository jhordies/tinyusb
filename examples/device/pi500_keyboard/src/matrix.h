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

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pi 500 keyboard matrix dimensions
#define MATRIX_ROWS 8
#define MATRIX_COLS 18

// Matrix pin definitions
extern const uint8_t row_pins[MATRIX_ROWS];
extern const uint8_t col_pins[MATRIX_COLS];

// Key mapping from matrix position to USB HID keycode
extern const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS];

// Matrix functions
void matrix_init(void);
void matrix_scan(void);
bool matrix_is_key_pressed(uint8_t row, uint8_t col);
uint8_t matrix_get_keycode(uint8_t row, uint8_t col);
void handle_power_key(bool pressed);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H_ */