/********************************
 * i2c_lcd.h
 *
 *  Created on: October 11, 2018
 *  Author: Hunter Hedges
 *
 ********************************/

#ifndef I2C_LCD_H_
#define I2C_LCD_H_

#define CHAR_WIDTH              5   // Chars are 5 bits wide
#define CHAR_HEIGHT             8   // Chars are 8 bits high

// Display commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// Text direction
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Display controls
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// Cursor shifts
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// Function controls
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_5x8DOTS             0x00

// Back light control
#define LCD_BACKLIGHT           0x08
#define LCD_NOBACKLIGHT         0x00

// Writing bits
#define ENABLE_BIT              0b00000100
#define READ_WRITE_BIT          0b00000010
#define REG_SELECT_BIT          0b00000001

/********************************
 * User Functions
 ********************************/
int LCD_init(uint8_t slaveAddress);
void LCD_clear(void);
void LCD_home(void);
void LCD_displayOn(void);
void LCD_displayOff(void);
int LCD_setCursorPosition(uint8_t row, uint8_t col);
void LCD_cursorOn(void);
void LCD_cursorOff(void);
void LCD_blinkOn(void);
void LCD_blinkOff(void);
void LCD_shiftDisplayLeft(void);
void LCD_shiftDisplayRight(void);
void LCD_textLeftToRight(void);
void LCD_textRightToLeft(void);
void LCD_autoscrollOn(void);
void LCD_autoscrollOff(void);
void LCD_backlightOn(void);
void LCD_backlightOff(void);
int LCD_isBacklightOn(void);
int LCD_createChar(uint8_t memAddress, uint8_t charMap[]);
void LCD_writeChar(uint8_t value);
void LCD_writeString(uint8_t * charBuffer, uint8_t numChars);

#endif /* I2C_LCD_H_ */
