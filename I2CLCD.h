/*
 * I2CLCD.h
 *
 *  Created on: Oct 11, 2018
 *      Author: Hunter
 */

#ifndef I2CLCD_H_
#define I2CLCD_H_

#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

#define LCD_BACKLIGHT           0x08
#define LCD_NOBACKLIGHT         0x00

#define ENABLE_BIT              0b00000100
#define READ_WRITE_BIT          0b00000010
#define REG_SELECT_BIT          0b00000001

typedef struct _LCD_Handle {
    uint8_t _addr;              // Slave address of device
    uint8_t _displayfunction;   // Device dependent 4BIT, 2LINE, 5x8DOTS, ect
    uint8_t _displaycontrol;    // Handles display and cursor
    uint8_t _displaymode;       // Handles direction of writing
    uint8_t _backlightval;
} LCD_Handle;

void _delayInit(void);
void _delayMicroseconds(uint32_t durationUs);
void LCD_init(uint8_t slaveAddress);
void LCD_begin(void);
void clear();
void home();
void display();
void cursor();
void noBlink();
void blink();
void command(uint8_t value);
void write(uint8_t value);
void send(uint8_t value, uint8_t mode);
void write4bits(uint8_t value);
void expanderWrite(uint8_t _data);
void pulseEnable(uint8_t _data);

#endif /* I2CLCD_H_ */
