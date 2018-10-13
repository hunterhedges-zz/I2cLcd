/*
 * usb.h
 *
 *  Created on: Sep 7, 2018
 *      Author: hhedg
 */

#ifndef USB_H_
#define USB_H_

void USB_init(void (*usbCallbackFxn)(uint8_t charReceived));
void USB_intHandler(void);
void USB_sendBuffer(uint8_t* bufferToSend, uint8_t numChars);

#endif /* USB_H_ */
