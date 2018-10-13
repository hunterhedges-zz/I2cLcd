/********************************
 * MSP432 with I2C LCD Example
 *
 * Created on: October 11, 2018
 * Author: Hunter H.
 *
 ********************************/

/********************************
 * Includes
 ********************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib.h"
#include "i2c_lcd.h"
#include "usb.h"

/********************************
 * File Specific Defines
 ********************************/
#define SLAVE_ADDRESS   0x27
#define HAPPYFACE_ADDR  1
#define HEART_ADDR      2
#define DUCK_ADDR       3
#define ENTER_KEY       13
#define BACK_KEY        8

/********************************
 * File Specific Functions
 ********************************/
void createCustomChars(void);
void usbCallbackFxn(uint8_t charReceived);

/********************************/
int main(void)
{
    // Disable Watchdog
    WDT_A_holdTimer();

    // Initialize USB at 9600 baud
    USB_init(&usbCallbackFxn);

    // Initialize LCD with slave address
    LCD_init(SLAVE_ADDRESS);

    // Add custom chars to CGRAM
    createCustomChars();

    while (1);
}

/********************************/
void createCustomChars(void)
{
    uint8_t happyFace[CHAR_HEIGHT] = { 0x00, 0x00, 0x0A, 0x00, 0x11, 0x0E, 0x00 };
    uint8_t heart[CHAR_HEIGHT]     = { 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00 };
    uint8_t duck[CHAR_HEIGHT]      = { 0x00, 0x0C, 0x1D, 0x0F, 0x0F, 0x06, 0x00 };

    // Store happy face in memory address 0, heart in memory address 1....
    LCD_createChar(HAPPYFACE_ADDR, happyFace);
    LCD_createChar(HEART_ADDR, heart);
    LCD_createChar(DUCK_ADDR, duck);

    // Creating custom char moves the cursor, we'll reset it
    LCD_clear();
    LCD_home();
}

/********************************
 * Called every time a character is received from USB
 *
 * Test the i2c_lcd functions
 *********************************/
void usbCallbackFxn(uint8_t charReceived)
{
    static char rxBuffer[32];
    static uint8_t rxPtr = 0;

    // Echo char received
    USB_sendBuffer(&charReceived, 1);

    if(rxPtr == 32)
    {
        rxPtr = 0;
    }

    if(charReceived == ENTER_KEY)
    {
        USB_sendBuffer("\n", 1);

        if(strcmp(rxBuffer, "HAPPY") == 0)
        {
            LCD_writeChar(HAPPYFACE_ADDR);
        }
        else if(strcmp(rxBuffer, "HEART") == 0)
        {
            LCD_writeChar(HEART_ADDR);
        }
        else if(strcmp(rxBuffer, "DUCK") == 0)
        {
            LCD_writeChar(DUCK_ADDR);
        }
        else if(strcmp(rxBuffer, "HOME") == 0)
        {
            LCD_home();
        }
        else if(strcmp(rxBuffer, "CLEAR") == 0)
        {
            LCD_clear();
        }
        else if(strcmp(rxBuffer, "DISPLAYON") == 0)
        {
            LCD_displayOn();
        }
        else if(strcmp(rxBuffer, "DISPLAYOFF") == 0)
        {
            LCD_displayOff();
        }
        else if(strcmp(rxBuffer, "CURSORON") == 0)
        {
            LCD_cursorOn();
        }
        else if(strcmp(rxBuffer, "CURSOROFF") == 0)
        {
            LCD_cursorOff();
        }
        else if(strcmp(rxBuffer, "BLINKON") == 0)
        {
            LCD_blinkOn();
        }
        else if(strcmp(rxBuffer, "BLINKOFF") == 0)
        {
            LCD_blinkOff();
        }
        else if(strcmp(rxBuffer, "SHIFTLEFT") == 0)
        {
            LCD_shiftDisplayLeft();
        }
        else if(strcmp(rxBuffer, "SHIFTRIGHT") == 0)
        {
            LCD_shiftDisplayRight();
        }
        else if(strcmp(rxBuffer, "TEXTTORIGHT") == 0)
        {
            LCD_textLeftToRight();
        }
        else if(strcmp(rxBuffer, "TEXTTOLEFT") == 0)
        {
            LCD_textRightToLeft();
        }
        else if(strcmp(rxBuffer, "AUTOSCROLLON") == 0)
        {
            LCD_autoscrollOn();
        }
        else if(strcmp(rxBuffer, "AUTOSCROLLOFF") == 0)
        {
            LCD_autoscrollOff();
        }
        else if(strcmp(rxBuffer, "BACKLIGHTON") == 0)
        {
            LCD_backlightOn();
        }
        else if(strcmp(rxBuffer, "BACKLIGHTOFF") == 0)
        {
            LCD_backlightOff();
        }
        else
        {
            LCD_writeString((uint8_t*)rxBuffer, rxPtr);
        }

        // Clear buffer
        memset(rxBuffer, 0, 32);
        rxPtr = 0;
    }
    else if(charReceived == BACK_KEY)
    {
        rxPtr--;
    }
    else
    {
        rxBuffer[rxPtr++] = (char)charReceived;
    }

}
