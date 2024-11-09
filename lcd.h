#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <pin_mux.h>
#include "fsl_gpio.h"

// Define to specify bit operation mode
// #define FOUR_BIT_MODE 0
#define EIGHT_BIT_MODE 0

#define ENABLE_TIME 1

// GPIO pin configurations for control signals
#define MB_RST_GPIO GPIOB  // Update this with the correct GPIO port
#define MB_RST_PIN  2      // Update this with the correct pin number
#define MB_CS1_GPIO GPIOC  // Update this with the correct GPIO port
#define MB_CS1_PIN  12     // Update this with the correct pin number
#define MB_CS2_GPIO GPIOC  // Update this with the correct GPIO port
#define MB_CS2_PIN  13     // Update this with the correct pin number

// Chip select definitions
#define CS1 1
#define CS2 2

// Function prototypes
void lcd_init(void);
void lcd_send_nibble(uint8_t nibble);
void lcd_delay(uint8_t delay_ms);
void lcd_enable(void);
void lcd_send_command(uint8_t command);
void lcd_print_char(char character);
void lcd_print_string(char *string);
void lcd_clear(void);
void lcd_wait_busy_flag(void);
void lcd_set_cursor(uint8_t page, uint8_t column, uint8_t chip);
void lcd_send_byte(uint8_t byte);
void lcd_send_command_to_chip(uint8_t command, uint8_t chip);

#endif /* LCD_H_ */
