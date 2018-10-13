/*
 * usb.c
 *
 *  Created on: Sep 7, 2018
 *      Author: hhedg
 */

#include <driverlib.h>
#include "usb.h"

/***************************
 * File Specific Defines
 ***************************/
#define PRIMARY_FUNCTION        GPIO_PRIMARY_MODULE_FUNCTION
#define USB_UART_PINS           GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3
#define USB_UART_BASE           EUSCI_A0_BASE
#define RX_INTERRUPT            EUSCI_A_UART_RECEIVE_INTERRUPT
#define TX_INTERRUPT            EUSCI_A_UART_TRANSMIT_INTERRUPT
#define USB_UART_INT            INT_EUSCIA0

/***************************
 * UART Configuration based on TI Tool:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 ***************************/
const eUSCI_UART_Config usbUARTConfig =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
    19,                                             // BRDIV = 19
    8,                                              // UCxBRF = 8
    85,                                             // UCxBRS = 85
    EUSCI_A_UART_NO_PARITY,                         // No Parity
    EUSCI_A_UART_LSB_FIRST,                         // MSB First
    EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
    EUSCI_A_UART_MODE,                              // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION   // Over sampling
};

/***************************
 * Function to be called on receiving character from USB
 ***************************/
void (*_usbCallbackFxn)(uint8_t charReceived);

/***************************
 * Setup USB UART
 ***************************/
void USB_init(void (*usbCallbackFxn)(uint8_t charReceived))
{
    GPIO_setAsPeripheralModuleFunctionInputPin(USB_UART_PINS, PRIMARY_FUNCTION);

    UART_initModule(USB_UART_BASE, &usbUARTConfig);

    UART_enableModule(USB_UART_BASE);

    UART_enableInterrupt(USB_UART_BASE, RX_INTERRUPT);

    Interrupt_enableInterrupt(USB_UART_INT);

    _usbCallbackFxn = usbCallbackFxn;
}

/***************************
 * Triggered on all EUSCI_A0 UART interrupts
 * Call callbackFxn when character is received
 * No echo
 ***************************/
void USB_intHandler(void)
{
    uint32_t intStatus = UART_getEnabledInterruptStatus(USB_UART_BASE);

    if(intStatus == RX_INTERRUPT)
    {
        UART_clearInterruptFlag(USB_UART_BASE, RX_INTERRUPT);

        uint8_t charReceived = UART_receiveData(USB_UART_BASE);

        _usbCallbackFxn(charReceived);
    }
}

/***************************/
void USB_sendBuffer(uint8_t* buffer, uint8_t bufferSize)
{
    int i;
    for(i = 0; i < bufferSize; i++)
    {
        UART_transmitData(USB_UART_BASE, buffer[i]);
    }
}
