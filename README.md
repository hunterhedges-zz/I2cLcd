# I2C LCD Library for MSP432

#### Easily setup and control a 16x2 LCD screen using an I2C interface!<br>
<img src="https://raw.githubusercontent.com/hunterhedges/I2cLcd/master/pics/photo_2.jpg" width="363" height="272">

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
1. Connect Pin 1.6 on MSP432 to LCD SDA with a pullup resistor (around 10KOhm).
2. Connect Pin 1.7 on MSP432 to LCD SCL with a pullup resistor (around 10KOhm).

3. Connect to MSP432 using USB and a serial connection application like Tera Term at 9600 baud.

4. Writing to the MSP432 will then pass characters to the LCD when the enter key is pressed. 

## Special Functions:
##### *These must be typed in all CAPS*

**HAPPY**<br>
Displays a happy face

**HEART**<br>
Displays a heart

**DUCK**<br> 
Displays a duck

**HOME**<br>
Moves the cursor to first position

**CLEAR**<br>
Clears the screen and moves cursor home

**DISPLAYON**<br>
Turn on the display

**DISPLAYOFF**<br>
Turn off the display

**CURSORON**<br>
Turns on the cursor

**CURSOROFF**<br>
Turns off the cursor

**BLINKON**<br>
Turns cursor blinking on

**BLINKOFF**<br>
Turns cursor blinking off

**SHIFTLEFT**<br>
Shifts all text one to left

**SHIFTRIGHT**<br>
Shifts all text one to right

**TEXTTORIGHT**<br>
Text will be written left to right

**TEXTTOLEFT**<br>
Text will be written right to left

**AUTOSCROLLON**<br> 
Shifts all text each character written

**AUTOSCROLLOFF**<br>
Moves cursor when a character is written

**BACKLIGHTON**<br>
Turns on the backlight

**BACKLIGHTOFF**<br>
Turns off the backlight



