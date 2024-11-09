#include "lcd.h"

// *****************************************************************************************************************
// Description: Initializes the NHD-12864AZ-NSW-BBW-TR LCD with the AiP31108U controller
void lcd_init(void) {
    lcd_delay(230);
    #ifdef FOUR_BIT_MODE
    lcd_send_nibble(0x02);  // Initialize in 4-bit mode
    lcd_wait_busy_flag();
    lcd_send_command(0x28); // Function set: 4-bit mode, 2 lines, 5x8 font
    lcd_wait_busy_flag();
    lcd_send_command(0x14); // Internal osc frequency
    lcd_wait_busy_flag();
    lcd_send_command(0x0F); // Display ON, cursor ON, blink ON
    lcd_wait_busy_flag();
    lcd_send_command(0x01); // Clear display
    lcd_wait_busy_flag();
    lcd_send_command(0x06); // Entry mode set: Increment cursor, no shift
    lcd_wait_busy_flag();
    #endif

    #ifdef EIGHT_BIT_MODE
    lcd_send_command(0x38); // Function set: 8-bit mode, 2 lines, 5x8 font
    lcd_wait_busy_flag();
    lcd_send_command(0x14); // Internal osc frequency
    lcd_wait_busy_flag();
    lcd_send_command(0x0F); // Display ON, cursor ON, blink ON
    lcd_wait_busy_flag();
    lcd_send_command(0x01); // Clear display
    lcd_wait_busy_flag();
    lcd_send_command(0x06); // Entry mode set: Increment cursor, no shift
    lcd_wait_busy_flag();
    #endif
    lcd_delay(230);
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Writes a four-bit nibble
void lcd_send_nibble(uint8_t nibble) {
    GPIO_PinWrite(MB_D7_GPIO, MB_D7_PIN, (nibble >> 3) & 0x01);
    GPIO_PinWrite(MB_D6_GPIO, MB_D6_PIN, (nibble >> 2) & 0x01);
    GPIO_PinWrite(MB_D5_GPIO, MB_D5_PIN, (nibble >> 1) & 0x01);
    GPIO_PinWrite(MB_D4_GPIO, MB_D4_PIN, (nibble >> 0) & 0x01);

    lcd_enable();
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Reads the busy flag
void lcd_wait_busy_flag(void) {
    gpio_pin_config_t data_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    GPIO_PinInit(MB_D7_GPIO, MB_D7_PIN, &data_config); // D7 is input (busy flag)
    GPIO_PinInit(MB_D6_GPIO, MB_D6_PIN, &data_config);
    GPIO_PinInit(MB_D5_GPIO, MB_D5_PIN, &data_config);
    GPIO_PinInit(MB_D4_GPIO, MB_D4_PIN, &data_config);

    #ifdef EIGHT_BIT_MODE
    GPIO_PinInit(MB_D3_GPIO, MB_D3_PIN, &data_config);
    GPIO_PinInit(MB_D2_GPIO, MB_D2_PIN, &data_config);
    GPIO_PinInit(MB_D1_GPIO, MB_D1_PIN, &data_config);
    GPIO_PinInit(MB_D0_GPIO, MB_D0_PIN, &data_config);
    #endif

    // RS = 0, RW = 1, E = 0
    GPIO_PinWrite(MB_RS_GPIO, MB_RS_PIN, 0U);
    GPIO_PinWrite(MB_RW_GPIO, MB_RW_PIN, 1U);
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U);

    while (1) { // Exit loop when busy flag is reset
        GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 1U); // Set Enable
        lcd_delay(ENABLE_TIME);

        // Read busy flag (D7)
        if (GPIO_PinRead(MB_D7_GPIO, MB_D7_PIN) == 0) {
            GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U); // Clear Enable
            break;
        }

        GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U); // Clear Enable
        lcd_delay(ENABLE_TIME);
    }

    data_config.pinDirection = kGPIO_DigitalOutput;
    GPIO_PinInit(MB_D7_GPIO, MB_D7_PIN, &data_config);
    GPIO_PinInit(MB_D6_GPIO, MB_D6_PIN, &data_config);
    GPIO_PinInit(MB_D5_GPIO, MB_D5_PIN, &data_config);
    GPIO_PinInit(MB_D4_GPIO, MB_D4_PIN, &data_config);
    #ifdef EIGHT_BIT_MODE
    GPIO_PinInit(MB_D3_GPIO, MB_D3_PIN, &data_config);
    GPIO_PinInit(MB_D2_GPIO, MB_D2_PIN, &data_config);
    GPIO_PinInit(MB_D1_GPIO, MB_D1_PIN, &data_config);
    GPIO_PinInit(MB_D0_GPIO, MB_D0_PIN, &data_config);
    #endif
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Toggles the enable pin to latch commands or data
void lcd_enable(void) {
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 1U);
    lcd_delay(ENABLE_TIME);
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U);
    lcd_delay(ENABLE_TIME);
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Generates a delay in milliseconds
void lcd_delay(uint8_t delay_ms) {
    volatile uint32_t i, j;
    for (i = 0; i < delay_ms; i++) {
        for (j = 0; j < 4000; j++) {
            __asm("NOP");
        }
    }
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Sends an 8-bit command to the LCD
void lcd_send_command(uint8_t command) {
    GPIO_PinWrite(MB_RS_GPIO, MB_RS_PIN, 0U); // Command mode
    GPIO_PinWrite(MB_RW_GPIO, MB_RW_PIN, 0U); // Write mode
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U);

    #ifdef FOUR_BIT_MODE
    lcd_send_nibble(command >> 4); // High nibble
    lcd_send_nibble(command & 0x0F); // Low nibble
    #endif

    #ifdef EIGHT_BIT_MODE
    lcd_send_byte(command);
    #endif
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Sends a full byte to the LCD
void lcd_send_byte(uint8_t byte) {
    GPIO_PinWrite(MB_D7_GPIO, MB_D7_PIN, (byte >> 7) & 0x01);
    GPIO_PinWrite(MB_D6_GPIO, MB_D6_PIN, (byte >> 6) & 0x01);
    GPIO_PinWrite(MB_D5_GPIO, MB_D5_PIN, (byte >> 5) & 0x01);
    GPIO_PinWrite(MB_D4_GPIO, MB_D4_PIN, (byte >> 4) & 0x01);
    GPIO_PinWrite(MB_D3_GPIO, MB_D3_PIN, (byte >> 3) & 0x01);
    GPIO_PinWrite(MB_D2_GPIO, MB_D2_PIN, (byte >> 2) & 0x01);
    GPIO_PinWrite(MB_D1_GPIO, MB_D1_PIN, (byte >> 1) & 0x01);
    GPIO_PinWrite(MB_D0_GPIO, MB_D0_PIN, (byte >> 0) & 0x01);

    lcd_enable();
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Prints a character at the current cursor position
void lcd_print_char(char character) {
    GPIO_PinWrite(MB_RS_GPIO, MB_RS_PIN, 1U); // Data mode
    GPIO_PinWrite(MB_RW_GPIO, MB_RW_PIN, 0U); // Write mode
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U);

    #ifdef FOUR_BIT_MODE
    lcd_send_nibble((uint8_t)character >> 4); // High nibble
    lcd_send_nibble((uint8_t)character & 0x0F); // Low nibble
    lcd_wait_busy_flag();
    #endif

    #ifdef EIGHT_BIT_MODE
    lcd_send_byte(character);
    lcd_wait_busy_flag();
    #endif
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: Prints a string on the LCD
// Description: prints a string
void lcd_print_string(char *string) {
	for(int i = 0; string[i] != '\0'; i++) {
		lcd_print_char(string[i]);
	}
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: clears display and sets DDRAM address 0 in address counter
void lcd_clear(void) {
	lcd_send_command(0x01);
	lcd_wait_busy_flag();
}
// *****************************************************************************************************************

// *****************************************************************************************************************
// Description: sets cursor to the given address
void lcd_set_cursor(uint8_t page, uint8_t column, uint8_t chip) {
    lcd_send_command_to_chip(0xB8 | (page & 0x07), chip); // Set page address
    lcd_send_command_to_chip(0x40 | (column & 0x7F), chip); // Set column address
}
// Description: sends a command to a specific chip (CS1 or CS2)
void lcd_send_command_to_chip(uint8_t command, uint8_t chip) {
    GPIO_PinWrite(MB_RS_GPIO, MB_RS_PIN, 0U);
    GPIO_PinWrite(MB_RW_GPIO, MB_RW_PIN, 0U);
    GPIO_PinWrite(MB_E_GPIO, MB_E_PIN, 0U);

    if (chip == CS1) {
        GPIO_PinWrite(MB_CS1_GPIO, MB_CS1_PIN, 1U);
        GPIO_PinWrite(MB_CS2_GPIO, MB_CS2_PIN, 0U);
    } else if (chip == CS2) {
        GPIO_PinWrite(MB_CS1_GPIO, MB_CS1_PIN, 0U);
        GPIO_PinWrite(MB_CS2_GPIO, MB_CS2_PIN, 1U);
    }

    lcd_send_byte(command);

    // Reset chip selection
    GPIO_PinWrite(MB_CS1_GPIO, MB_CS1_PIN, 0U);
    GPIO_PinWrite(MB_CS2_GPIO, MB_CS2_PIN, 0U);
}
// *****************************************************************************************************************
