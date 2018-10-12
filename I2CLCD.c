/*
 * I2CLCD.c
 *
 *  Created on: Oct 11, 2018
 *      Author: Hunter
 */

#include <stdbool.h>
#include <stdint.h>
#include <driverlib.h>
#include "I2CLCD.h"

    uint8_t _addr;              // Slave address of device
    uint8_t _displayfunction;   // Device dependent 4BIT, 2LINE, 5x8DOTS, ect
    uint8_t _displaycontrol;    // Handles display and cursor
    uint8_t _displaymode;       // Handles direction of writing
    uint8_t _backlightval = LCD_BACKLIGHT;

//
//
//
void _delayInit(void)
{
    Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1,
                           TIMER32_32BIT, TIMER32_PERIODIC_MODE);
}

//
//
//
void _delayMicroseconds(uint32_t durationUs)
{
    uint32_t freq = CS_getMCLK();
    durationUs = durationUs * 48;

    Timer32_setCount(TIMER32_BASE, durationUs);
    Timer32_startTimer(TIMER32_BASE, true);

    while(Timer32_getValue(TIMER32_BASE) != 0);

    Timer32_haltTimer(TIMER32_BASE);
}

//
//
//
void LCD_init(uint8_t slaveAddress)
{
    _addr = slaveAddress;
    _delayInit();
    LCD_begin();
}

//
//
//
void LCD_begin()
{
    // Default device function
    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
    // According to datasheet, we need at least 40ms after power rises above 2.7V
    _delayMicroseconds(50 * 1000);

    // Now we pull both RS and R/W low to begin commands
    expanderWrite(_backlightval);   // reset expander and turn backlight off (Bit 8 =1)
    _delayMicroseconds(1000 * 1000);

    // Put the LCD into 4 bit mode
    // According to Figure 24, page 46 of datasheet

    // We start in 8 bit mode, try to set 4 bit mode
    write4bits(0x03 << 4);
    _delayMicroseconds(45 * 100); // Wait more than 4.1ms

    // Second try
    write4bits(0x03 << 4);
    _delayMicroseconds(150); // Wait more than 100us

    // Third try
    write4bits(0x03 << 4);

    // Finally, set to 4-bit interface
    write4bits(0x02 << 4);

    // Set number of lines, font size, ect
    command(LCD_FUNCTIONSET | _displayfunction);

    // Turn the display on with blinking cursor for default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;
    display();

    // Clear it off
    clear();

    // Initialize to default text direction
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // Set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);

    // Set cursor to start position
    home();
}

void clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    _delayMicroseconds(45 * 100); // This command takes a long time
}

void home()
{
    command(LCD_RETURNHOME);  // set cursor position to zero
    _delayMicroseconds(45 * 100); // This command takes a long time
}

void display()
{
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void cursor()
{
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void noBlink()
{
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void blink()
{
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void command(uint8_t value)
{
    send(value, 0);
}

void write(uint8_t value)
{
    send(value, REG_SELECT_BIT);
}

// write either command or data
void send(uint8_t value, uint8_t mode)
{
    uint8_t highnib=value&0xf0;
    uint8_t lownib=(value<<4)&0xf0;
    write4bits((highnib)|mode);
    write4bits((lownib)|mode);
}

void write4bits(uint8_t value)
{
    expanderWrite(value);
    pulseEnable(value);
}

void expanderWrite(uint8_t _data)
{
    I2C_masterSendSingleByte(EUSCI_B0_BASE, _data | _backlightval);
}

void pulseEnable(uint8_t _data)
{
    expanderWrite(_data | ENABLE_BIT);  // En high
    _delayMicroseconds(1); // enable pulse must be >450ns

    expanderWrite(_data & ~ENABLE_BIT); // En low
    _delayMicroseconds(50); // commands need > 37us to settle
}
