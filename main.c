/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    LCD_PROJECT.c
 * @brief   Main entry point for LCD state machine application.
 */
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "lcd.h"
#include <stdint.h>
#include <stdlib.h> // For atoi

/* Function prototypes */
void state_machine_init(void);
void state_init(void);
void state_clear(void);
void state_write(void);
void state_set_cursor(void);
void state_idle(void);
char scan_keypad(void);

/* State enumeration */
typedef enum {
    STATE_INIT = 0,
    STATE_CLEAR,
    STATE_WRITE,
    STATE_CURSOR,
    STATE_IDLE
} State;

State current_state;

/* State function table */
static void (*state_functions[])(void) = {
    state_init,
    state_clear,
    state_write,
    state_set_cursor,
    state_idle
};

char key;

#define NUM_ROWS 4
#define NUM_COLS 4

const uint8_t row_pins[NUM_ROWS] = {11U, 12U, 13U, 14U};
const uint8_t col_pins[NUM_COLS] = {12U, 13U, 14U, 15U};

const char key_map[NUM_ROWS][NUM_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/*
 * @brief   Main function for the LCD project.
 */
int main(void) {
    /* Initialize board hardware */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    MB_InitMC_PWMPins();

    state_machine_init();

    /* Main loop */
    while (1) {
        key = scan_keypad();
        state_functions[current_state]();
    }

    return 0;
}

void state_machine_init(void) {
    current_state = STATE_INIT;
}

char scan_keypad(void) {
    for (uint8_t row = 0; row < NUM_ROWS; row++) {
        // Set all rows high
        for (uint8_t i = 0; i < NUM_ROWS; i++) {
            GPIO_PinWrite(GPIOA, row_pins[i], 1U);
        }

        // Set current row low
        GPIO_PinWrite(GPIOA, row_pins[row], 0U);

        // Small stabilization delay
        SDK_DelayAtLeastUs(5U, CLOCK_GetFreq(kCLOCK_CoreSysClk));

        // Scan columns
        for (uint8_t col = 0; col < NUM_COLS; col++) {
            if (!GPIO_PinRead(GPIOB, col_pins[col])) {
                // Debounce delay
                SDK_DelayAtLeastUs(20U, CLOCK_GetFreq(kCLOCK_CoreSysClk));

                // Verify key press
                if (!GPIO_PinRead(GPIOB, col_pins[col])) {
                    // Wait for key release
                    while (!GPIO_PinRead(GPIOB, col_pins[col])) {
                        SDK_DelayAtLeastUs(5U, CLOCK_GetFreq(kCLOCK_CoreSysClk));
                    }
                    return key_map[row][col];
                }
            }
        }
    }
    return '\0'; // No key pressed
}

void state_init(void) {
    lcd_init();
    lcd_print_string("Hello");
    current_state = STATE_IDLE;
}

void state_clear(void) {
    lcd_clear();
    current_state = STATE_IDLE;
}

void state_write(void) {
    char input_buffer[8] = {0};
    char c;
    int index = 0;

    while (1) {
        c = scan_keypad();
        if (c == '*') {
            lcd_print_string(input_buffer);
            break;
        }
        if (c == '#') {
            break;
        }
        if (c != '\0' && index < sizeof(input_buffer) - 1) {
            input_buffer[index++] = c;
        }
    }
    current_state = STATE_IDLE;
}

void state_set_cursor(void) {
    char input_buffer[8] = {0};
    char c;
    int index = 0;

    while (1) {
        c = scan_keypad();
        if (c == '*') {
            lcd_set_cursor(atoi(input_buffer), 0, CS1);
            break;
        }
        if (c == '#') {
            break;
        }
        if (c != '\0' && index < sizeof(input_buffer) - 1) {
            input_buffer[index++] = c;
        }
    }
    current_state = STATE_IDLE;
}

void state_idle(void) {
    switch (key) {
        case 'A': current_state = STATE_WRITE; break;
        case 'C': current_state = STATE_CLEAR; break;
        case 'D': current_state = STATE_CURSOR; break;
        default: current_state = STATE_IDLE;
    }
}
