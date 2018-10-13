# I2C LCD Library for MSP432

Easily setup and control a 16x2 LCD screen using an I2C interface

								 5V   5V
                                 /|\  /|\
                 MSP432P401     ~10k ~10k     LCD with I2C
                   Master         |    |         Slave
              -----------------   |    |   -----------------
             |     P1.6/UCB0SDA|<-|----+->|SDA              |
             |                 |  |       |                 |
             |                 |  |       |                 |
             |     P1.7/UCB0SCL|<-+------>|SCL              |
             |                 |          |                 |

## To Try The Example
Connect Pin 1.6 on MSP432 to LCD SDA with a pullup resistor.
Connect Pin 1.7 on MSP432 to LCD SCL with a pullup resistor.

Connect to MSP432 using USB and a serial connection application like Tera Term at 9600 baud.

Writing to the MSP432 will then pass characters to the LCD when the enter key is pressed. 

To test out library functions such as turning the backlight on or off,
type one of the functions below in all CAPS.

List of functions to try:
HAPPY - displays a happy face
HEART - displays a heart
DUCK - display a duck
HOME - moves the cursor to first position
CLEAR - clears the screen and moves cursor home
DISPLAYON - turn on the display
DISPLAYOFF - turn off the display
CURSORON - turns on the cursor
CURSOROFF - turns off the cursor
BLINKON - turns cursor blinking on
BLINKOFF - turns cursor blinking off
SHIFTLEFT - shifts all text one to left
SHIFTRIGHT - shifts all text one to right
TEXTTORIGHT - text will be written left to right
TEXTTOLEFT - text will be written right to left
AUTOSCROLLON - shifts all text each character written
AUTOSCROLLOFF - moves cursor when a character is written
BACKLIGHTON - turns on the backlight
BACKLIGHTOFF - turns off the backlight


