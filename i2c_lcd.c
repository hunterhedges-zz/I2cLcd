/****************************************************************
 * i2c_lcd.c
 *
 *  Created on: October 11, 2018
 *  Author: Hunter H.
 *
 *  This file abstracts the writing to an 16x2 LCD display
 *  that uses I2C for data transfer. Specifically used
 *  for a TI MSP432.
 *
 *  Based on the data sheet found here:
 *  https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 *
 *  With help from the many Arduino I2C libraries.
 *
 *                                5V   5V
 *                                /|\  /|\
 *                MSP432P401     ~10k ~10k     LCD with I2C
 *                  Master         |    |         slave
 *             -----------------   |    |   -----------------
 *            |     P1.6/UCB0SDA|<-|----+->|SDA              |
 *            |                 |  |       |                 |
 *            |                 |  |       |                 |
 *            |     P1.7/UCB0SCL|<-+------>|SCL              |
 *            |                 |          |                 |
 ****************************************************************/

// SET THIS TO THE CORRECT CLOCK FREQUENCY
// Defaulted to 3MHz (MSP432 default)
#define CLOCK_FREQ  3000000

/********************************
 * Includes
 ********************************/
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <driverlib.h>
#include "i2c_lcd.h"

/********************************
 * File specific functions
 ********************************/
static void _delayInit(void);
static void _delayMicroseconds(uint32_t durationUs);
static void _i2cInit(uint8_t slaveAddress);
static void _command(uint8_t value);
static void _send(uint8_t value, uint8_t mode);
static void _write4bits(uint8_t value);
static void _expanderWrite(uint8_t _data);
static void _pulseEnable(uint8_t _data);

/********************************
 * Global variables specific to file
 ********************************/
static uint8_t _displayControl; // Handles display and cursor
static uint8_t _displayMode;    // Handles the direction of text
static uint8_t _backlightVal;   // Whether back light is on or off

// I2C Master Configuration
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        CLOCK_FREQ,                             // Set the Clock Frequency
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100KHz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

/********************************
 * We want to use the TIMER32 on the MSP432
 * because this will allow us to use the same
 * code with different clock frequencies easily
 *
 * Changes made to the TIMER32 could affect the LCD
 ********************************/
static void _delayInit(void)
{
    Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1,
                           TIMER32_32BIT, TIMER32_PERIODIC_MODE);
}

static void _delayMicroseconds(uint32_t durationUs)
{
    durationUs = durationUs * (CLOCK_FREQ / 1000000);

    Timer32_setCount(TIMER32_BASE, durationUs);
    Timer32_startTimer(TIMER32_BASE, true);

    while(Timer32_getValue(TIMER32_BASE) != 0);

    Timer32_haltTimer(TIMER32_BASE);
}

/********************************
 * Initializes the I2C EUSCI_B0 on the MSP432
 * Pin 1.6 = SDA (Data)
 * Pin 1.7 = SCL (Clock)
 ********************************/
static void _i2cInit(uint8_t slaveAddress)
{
    // Selects Port 1 for I2c (1.6 = SDA, 1.7 = SCL)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Initialize based on config
    I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

    // Specify slave address of LCD
    I2C_setSlaveAddress(EUSCI_B0_BASE, slaveAddress);

    // Set master in transmit mode
    I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Enable to start operation
    I2C_enableModule(EUSCI_B0_BASE);
}

/********************************
 * Code created according to the data sheet (page 45/46)
 * https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 *
 * Param: Slave address of LCD
 * Returns: 1 on success, 0 otherwise
 ********************************/
int LCD_init(uint8_t slaveAddress)
{
    // Make sure the CLOCK_FREQ definition matches actual clock frequency
    uint32_t clockFreq = CS_getSMCLK();
    if(CLOCK_FREQ != clockFreq)
    {
        // Failed
        return 0;
    }

    _i2cInit(slaveAddress);
    _delayInit();

    // Default display, text direction, and back light
    uint8_t displayFunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    _backlightVal = LCD_BACKLIGHT;

    // We need at least 40ms after power rises above 2.7V
    _delayMicroseconds(50 * 1000);

    // Now we pull both RS and R/W low to begin commands
    _expanderWrite(_backlightVal);

    // We start in 8 bit mode, try to set 4 bit mode
    _write4bits(0x03 << 4);
    _delayMicroseconds(45 * 100); // Wait more than 4.1ms

    // Second try
    _write4bits(0x03 << 4);
    _delayMicroseconds(150); // Wait more than 100us

    // Third try
    _write4bits(0x03 << 4);

    // Finally, set to 4-bit interface
    _write4bits(0x02 << 4);

    // Set number of lines, font size...
    _command(LCD_FUNCTIONSET | displayFunction);

    // Turn the display on with blinking cursor for default
    _displayControl = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;
    LCD_displayOn();

    // Clear it off
    LCD_clear();

    // Set the entry mode
    _command(LCD_ENTRYMODESET | _displayMode);

    // Set cursor to start position
    LCD_home();

    return 1;
}

/********************************
 * Clear display and set cursor position to zero
 * This command takes a long time (delay needed)
 ********************************/
void LCD_clear(void)
{
    _command(LCD_CLEARDISPLAY);
    _delayMicroseconds(45 * 100);
}

/********************************
 * Set cursor position to zero
 * This command takes a long time (delay needed)
 ********************************/
void LCD_home(void)
{
    _command(LCD_RETURNHOME);
    _delayMicroseconds(45 * 100);
}

/********************************
 * Turn the display on or off
 * This isn't the same as turning on/off the backlight
 ********************************/
void LCD_displayOn(void)
{
    _displayControl |= LCD_DISPLAYON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

void LCD_displayOff(void)
{
    _displayControl &= ~LCD_DISPLAYON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

/********************************
 * Set the location of the cursor assuming 16x2 LCD
 * Numbering is based on zero indexed arrays
 * For example, rows are 0 or 1 (top or bottom)
 ********************************/
int LCD_setCursorPosition(uint8_t row, uint8_t col)
{
   // Sanity check row and columns...
   // No need to check less than 0 on unsigned byte
   if(row > 1 || col > 16)
   {
       return 0;
   }

   int row_offsets[] = { 0x00, 0x40 };
   _command(LCD_SETDDRAMADDR | (col + row_offsets[row]));

   return 1;
}

/********************************
 * Display or hide the cursor
 ********************************/
void LCD_cursorOn(void)
{
    _displayControl |= LCD_CURSORON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

void LCD_cursorOff(void)
{
    _displayControl &= ~LCD_CURSORON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

/********************************
 * Turn blinking cursor on/off
 ********************************/
void LCD_blinkOn(void)
{
    _displayControl |= LCD_BLINKON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

void LCD_blinkOff(void)
{
    _displayControl &= ~LCD_BLINKON;
    _command(LCD_DISPLAYCONTROL | _displayControl);
}

/********************************
 * Shifts the text on display 1 position to right or left
 * These functions will wrap text (they'll come out other side)
 ********************************/
void LCD_shiftDisplayLeft(void)
{
    _command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCD_shiftDisplayRight(void)
{
    _command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/********************************
 * Set the direction or writing text to
 * left -> right or right -> left
 *
 * Default is left to right
 ********************************/
void LCD_textLeftToRight(void)
{
    _displayMode |= LCD_ENTRYLEFT;
    _command(LCD_ENTRYMODESET | _displayMode);
}

void LCD_textRightToLeft(void)
{
    _displayMode &= ~LCD_ENTRYLEFT;
    _command(LCD_ENTRYMODESET | _displayMode);
}

/********************************
 * This will right justify text if on
 * Or left justify if off
 ********************************/
void LCD_autoscrollOn(void)
{
    _displayMode |= LCD_ENTRYSHIFTINCREMENT;
    _command(LCD_ENTRYMODESET | _displayMode);
}

void LCD_autoscrollOff(void)
{
    _displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
    _command(LCD_ENTRYMODESET | _displayMode);
}

/********************************
 * Turn the back light on or off
 * Default is on
 ********************************/
void LCD_backlightOn(void)
{
    _backlightVal = LCD_BACKLIGHT;
    _expanderWrite(0);
}

void LCD_backlightOff(void)
{
    _backlightVal = LCD_NOBACKLIGHT;
    _expanderWrite(0);
}

/********************************
 * Returns 1 if back light is on, 0 if off
 ********************************/
int LCD_isBacklightOn(void)
{
    if(_backlightVal == LCD_BACKLIGHT)
    {
        return 1;
    }

    return 0;
}

/********************************
 * We can create and store up to 8 custom characters to CGRAM
 * Look at data sheet starting at page 13
 *
 * memAddress: Location in EEPROM (0 - 7)
 * charMap: Array of uint8 that represent custom character
 * Only the 5 least significant bits are looked at
 *
 * Example using a heart :
 * uint8_t heart[CHAR_HEIGHT] = { 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00 };
 * [ 0 0 0 0 0      -> 0x00 = 0b00000
 *   0 1 0 1 0      -> 0x0A = 0b01010
 *   1 1 1 1 1      -> 0x1F = 0b11111
 *   1 1 1 1 1      -> 0x1F = 0b11111
 *   0 1 1 1 0      -> 0x0E = 0b01110
 *   0 0 1 0 0      -> 0x04 = 0b00100
 *   0 0 0 0 0 ]    -> 0x00 = 0b00000
 *   0 0 0 0 0 (Cursor)
 *
 *   Last row of bits (8th row) is dedicated to cursor
 *   Custom char generator: https://omerk.github.io/lcdchargen/
 *
 ********************************/
int LCD_createChar(uint8_t memAddress, uint8_t charMap[])
{
    // Sanity check that address is between 0 - 7
    if(memAddress > 7)
    {
        // Failed
        return 0;
    }

    // Send mask of CGRAM address and location shifted 3 bits (page 19)
    _command(LCD_SETCGRAMADDR | (memAddress << 3));

    // Write each line of bits
    int i;
    for(i = 0; i < CHAR_HEIGHT; i++)
    {
        LCD_writeChar(charMap[i]);
    }

    return 1;
}

/********************************
 * Writes a single character to the LCD
 * Wraps the cursor if position isn't on screen
 ********************************/
void LCD_writeChar(uint8_t value)
{
    _send(value, REG_SELECT_BIT);
}

/********************************
 * Writes a string to the LCD
 ********************************/
void LCD_writeString(uint8_t * charBuffer, uint8_t numChars)
{
    int i;
    for(i = 0; i < numChars; i++)
    {
        LCD_writeChar(charBuffer[i]);
    }
}

/********************************
 * Handles the processing of display commands
 * Not related to writing characters to screen
 * We'll make this internal for abstraction
 ********************************/
static void _command(uint8_t value)
{
    _send(value, 0);
}

/********************************
 * Processing for sending data for 4 bit mode
 ********************************/
static void _send(uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    _write4bits(highnib | mode);
    _write4bits(lownib | mode);
}

/********************************/
static void _write4bits(uint8_t value)
{
    _expanderWrite(value);
    _pulseEnable(value);
}

/********************************
 * Write single byte to I2C data line
 ********************************/
static void _expanderWrite(uint8_t data)
{
    I2C_masterSendSingleByte(EUSCI_B0_BASE, data | _backlightVal);
}

/********************************/
static void _pulseEnable(uint8_t data)
{
    _expanderWrite(data | ENABLE_BIT);  // Enable bit high
    _delayMicroseconds(1);              // Enable pulse must be >450ns

    _expanderWrite(data & ~ENABLE_BIT); // Enable bit low
    _delayMicroseconds(50);             // Command needs >37us to settle
}
