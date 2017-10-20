/*
 * usbLed.c
 *
 *  Created on: Oct 20, 2017
 *      Author: Andrew Tesler
 *      andrey@compulab.co.il
 *
 *      Will collect all the functions related to the LEDS
 *      of the fit-statUSB device
 */

#include "usbLed.h"

/**
 * turn off all of the LED's
 */
void allOff() {

    GPIO_setAsInputPin(LED_PORT,LED_R);
    GPIO_setOutputHighOnPin(LED_PORT,LED_R);

    GPIO_setAsInputPin(LED_PORT,LED_G);
    GPIO_setOutputHighOnPin(LED_PORT,LED_G);

    GPIO_setAsInputPin(LED_PORT,LED_B);
    GPIO_setOutputHighOnPin(LED_PORT,LED_B);

}

/**
 * Turn on all of the LED's
 */
void allOn() {
    GPIO_setAsOutputPin(LED_PORT,LED_R);
    GPIO_setOutputLowOnPin(LED_PORT,LED_R);

    GPIO_setAsOutputPin(LED_PORT,LED_G);
    GPIO_setOutputLowOnPin(LED_PORT,LED_G);

    GPIO_setAsOutputPin(LED_PORT,LED_B);
    GPIO_setOutputLowOnPin(LED_PORT,LED_B);
}

void ledOn(char led) {

    allOff();                                                       // Initially turn off all of the leds

    switch(led) {
    case 'R' :
        GPIO_setAsOutputPin(LED_PORT,LED_R);
        GPIO_setOutputLowOnPin(LED_PORT,LED_R);
        break;
    case 'G' :
        break;
    case 'B' :
        printf("Well done\n" );
        break;
    case 'D' :
        printf("You passed\n" );
        break;
    case 'F' :
        printf("Better try again\n" );
        break;
    default :
        printf("Invalid LED\n" );
    }

}




