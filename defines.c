/*
 * defines.c
 *
 *  Created on: 23/12/2016
 *      Author: Andrew Tesler
 *      Global defines file
 */




//#define ARDUMSP
#define FITSTATUSB

#define GPIO_ALL    GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3| \
        GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7

#define LED_PORT	GPIO_PORT_P1
#define LED_G		GPIO_PIN4
#define LED_B		GPIO_PIN3
#define LED_R		GPIO_PIN2

#define MAX_FADE_DECIMAL 5                                                                              // Maximum fade decimal numbers, If increasing size check all related value size also
#define MAX_SEQ_COLORS   10                                                                            // Maximum colors to be entered in sequence

