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

// Pi 500 power button GPIO
#define PWR_BTN_GPIO 19
#define CAPS_LED_GPIO 25

// Power button state
static bool pwr_button_pressed = false;
static bool prev_power_key_state = false;

// Pi 500 keyboard matrix pin definitions
const uint8_t row_pins[MATRIX_ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
const uint8_t col_pins[MATRIX_COLS] = {27, 8, 9, 10, 11, 12, 13, 14, 23, 24, 22, 20, 29, 18, 15, 21, 28, 26};

// Matrix state
static bool matrix_state[MATRIX_ROWS][MATRIX_COLS];
static bool matrix_prev_state[MATRIX_ROWS][MATRIX_COLS];

// USB HID keycode mapping for Pi 500 keyboard layout
// Exact 1:1 mapping from QMK keyboard.json matrix coordinates
const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
  // Row 0: GP0 - [0,1]Q [0,2]W [0,3]E [0,4]R [0,5]U [0,6]I [0,7]O [0,8]P [0,10]PAUSE
  {0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, 0, HID_KEY_PAUSE, 0, 0, 0, 0, 0, 0, 0},
  
  // Row 1: GP1 - [1,1]TAB [1,2]CAPS [1,3]F3 [1,4]T [1,5]Y [1,6]] [1,7]F7 [1,8][ [1,10]BKSP [1,15]LSHIFT [1,16]LALT
  {0, HID_KEY_TAB, HID_KEY_CAPS_LOCK, HID_KEY_F3, HID_KEY_T, HID_KEY_Y, HID_KEY_BRACKET_RIGHT, HID_KEY_F7, HID_KEY_BRACKET_LEFT, 0, HID_KEY_BACKSPACE, 0, 0, 0, 0, HID_KEY_SHIFT_LEFT, HID_KEY_ALT_LEFT, 0},
  
  // Row 2: GP2 - [2,1]A [2,2]S [2,3]D [2,4]F [2,5]J [2,6]K [2,7]L [2,8]; [2,9]LWIN [2,10]\ [2,15]RSHIFT
  {0, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_GUI_LEFT, HID_KEY_BACKSLASH, 0, 0, 0, 0, HID_KEY_SHIFT_RIGHT, 0, 0},
  
  // Row 3: GP3 - [3,1]ESC [3,2]PRTSC [3,3]F4 [3,4]G [3,5]H [3,6]F6 [3,8]' [3,9]FN [3,11]SPACE [3,14]UP
  {0, HID_KEY_ESCAPE, HID_KEY_PRINT_SCREEN, HID_KEY_F4, HID_KEY_G, HID_KEY_H, HID_KEY_F6, 0, HID_KEY_APOSTROPHE, 0, 0, HID_KEY_SPACE, 0, 0, HID_KEY_ARROW_UP, 0, 0, 0},
  
  // Row 4: GP4 - [4,0]RCTRL [4,1]Z [4,2]X [4,3]C [4,4]V [4,5]M [4,6], [4,7]. [4,8]SCRLK [4,10]ENTER [4,11]F11
  {HID_KEY_CONTROL_RIGHT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SCROLL_LOCK, 0, HID_KEY_ENTER, HID_KEY_F11, 0, 0, 0, 0, 0, 0},
  
  // Row 5: GP5 - [5,1]HOME [5,2]PGUP [5,3]PGDN [5,4]B [5,6]INS [5,7]END [5,8]/ [5,9]RALT [5,11]DOWN [5,12]RIGHT [5,14]LEFT
  {0, HID_KEY_HOME, HID_KEY_PAGE_UP, HID_KEY_PAGE_DOWN, HID_KEY_B, 0, HID_KEY_INSERT, HID_KEY_END, HID_KEY_SLASH, HID_KEY_ALT_RIGHT, 0, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT, 0, HID_KEY_ARROW_LEFT, 0, 0, 0},
  
  // Row 6: GP6 - [6,0]LCTRL [6,1]` [6,2]F1 [6,3]F2 [6,4]5 [6,5]6 [6,6]= [6,7]F8 [6,8]- [6,10]F9 [6,11]DEL
  {HID_KEY_CONTROL_LEFT, HID_KEY_GRAVE, HID_KEY_F1, HID_KEY_F2, HID_KEY_5, HID_KEY_6, HID_KEY_EQUAL, HID_KEY_F8, HID_KEY_MINUS, 0, HID_KEY_F9, HID_KEY_DELETE, 0, 0, 0, 0, 0, 0},
  
  // Row 7: GP7 - [7,0]F5 [7,1]1 [7,2]2 [7,3]3 [7,4]4 [7,5]7 [7,6]8 [7,7]9 [7,8]0 [7,9]F12 [7,10]F10
  {HID_KEY_F5, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_F12, HID_KEY_F10, 0, 0, 0, 0, 0, 0, 0}
};

// Power key RPI_PWR is not in the matrix - it's handled by special GPIO detection
// QMK uses GP20 (col) and GP6 (row) for power key detection

// Power button control (same as QMK implementation)
static void press_power_button(bool press) {
  if (press && !pwr_button_pressed) {
    gpio_init(PWR_BTN_GPIO);
    gpio_set_dir(PWR_BTN_GPIO, GPIO_OUT);
    gpio_put(PWR_BTN_GPIO, 1); // 'Press' Power Button
    pwr_button_pressed = true;
  } else if (!press && pwr_button_pressed) {
    gpio_set_dir(PWR_BTN_GPIO, GPIO_IN); // 'Release' Power Button
    pwr_button_pressed = false;
  }
}

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
  
  // Initialize power key detection (same as QMK)
  // GP20 = Top Right (Power) Key Column, GP6 = Top Right (Power) Key Row
  gpio_init(20); // Power key column
  gpio_set_dir(20, GPIO_OUT);
  gpio_put(20, 1);
  gpio_init(6);  // Power key row
  gpio_set_dir(6, GPIO_IN);
  gpio_pull_up(6);
  
  // Initialize power button GPIO
  gpio_init(PWR_BTN_GPIO);
  gpio_set_dir(PWR_BTN_GPIO, GPIO_IN);
  
  // Initialize Caps Lock LED
  gpio_init(CAPS_LED_GPIO);
  gpio_set_dir(CAPS_LED_GPIO, GPIO_OUT);
  gpio_put(CAPS_LED_GPIO, 0);
  
  // Initialize matrix state
  for (int row = 0; row < MATRIX_ROWS; row++) {
    for (int col = 0; col < MATRIX_COLS; col++) {
      matrix_state[row][col] = false;
      matrix_prev_state[row][col] = false;
    }
  }
  
  // Check initial power key state (same as QMK)
  gpio_put(20, 0); // Set power key column low
  sleep_us(1);
  bool power_key_pressed = !gpio_get(6); // Read power key row
  gpio_put(20, 1); // Set power key column back high
  press_power_button(power_key_pressed);
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
  
  // Scan power key separately (same as QMK)
  gpio_put(20, 0); // Set power key column low
  sleep_us(1);
  bool power_key_pressed = !gpio_get(6); // Read power key row
  gpio_put(20, 1); // Set power key column back high
  
  // Handle power key press/release
  if (power_key_pressed && !prev_power_key_state) {
    press_power_button(true);
  } else if (!power_key_pressed && prev_power_key_state) {
    press_power_button(false);
  }
  prev_power_key_state = power_key_pressed;
}

bool matrix_is_key_pressed(uint8_t row, uint8_t col) {
  if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
  return matrix_state[row][col];
}

uint8_t matrix_get_keycode(uint8_t row, uint8_t col) {
  if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return 0;
  return keymap[row][col];
}

// Handle power button press/release
void handle_power_key(bool pressed) {
  if (pressed && !prev_power_key_state) {
    // Key just pressed
    press_power_button(true);
  } else if (!pressed && prev_power_key_state) {
    // Key just released
    press_power_button(false);
  }
  prev_power_key_state = pressed;
}