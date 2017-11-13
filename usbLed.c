/*
 * usbLed.c
 *
 *  Created on: Oct 20, 2017
 *      Author: Andrew Tesler
 *      andrey@compulab.co.il
 *
 *      Help function for turning the Leds on/off
 */

#include "usbLed.h"

/**
 * turn off all of the LED's
 */
void allOff() {

    GPIO_setAsOutputPin(LED_PORT,LED_R);
    GPIO_setOutputLowOnPin(LED_PORT,LED_R);
//    GPIO_setOutputHighOnPin(LED_PORT,LED_R);

    GPIO_setAsOutputPin(LED_PORT,LED_G);
    GPIO_setOutputLowOnPin(LED_PORT,LED_G);
//    GPIO_setOutputHighOnPin(LED_PORT,LED_G);

    GPIO_setAsOutputPin(LED_PORT,LED_B);
    GPIO_setOutputLowOnPin(LED_PORT,LED_B);
//    GPIO_setOutputHighOnPin(LED_PORT,LED_B);

}

/**
 * Turn on all of the LED's
 */
void allOn() {
    GPIO_setAsOutputPin(LED_PORT,LED_R);
//    GPIO_setOutputLowOnPin(LED_PORT,LED_R);
    GPIO_setOutputHighOnPin(LED_PORT,LED_R);

    GPIO_setAsOutputPin(LED_PORT,LED_G);
//    GPIO_setOutputLowOnPin(LED_PORT,LED_G);
    GPIO_setOutputHighOnPin(LED_PORT,LED_G);

    GPIO_setAsOutputPin(LED_PORT,LED_B);
//    GPIO_setOutputLowOnPin(LED_PORT,LED_B);
    GPIO_setOutputHighOnPin(LED_PORT,LED_B);

}


void ledOn(char led) {
    allOff();                                                       // Initially turn off all of the LEDs
    switch(led) {
    case 'R' :
        GPIO_setAsOutputPin(LED_PORT,LED_R);
//        GPIO_setOutputLowOnPin(LED_PORT,LED_R);
        GPIO_setOutputHighOnPin(LED_PORT,LED_R);
        break;
    case 'G' :
        GPIO_setAsOutputPin(LED_PORT,LED_G);
//        GPIO_setOutputLowOnPin(LED_PORT,LED_G);
        GPIO_setOutputHighOnPin(LED_PORT,LED_G);
        break;
    case 'B' :
        GPIO_setAsOutputPin(LED_PORT,LED_B);
//        GPIO_setOutputLowOnPin(LED_PORT,LED_B);
        GPIO_setOutputHighOnPin(LED_PORT,LED_B);
        break;
    case 'S' : // SAGOL
        GPIO_setAsOutputPin(LED_PORT,LED_R);
        GPIO_setOutputLowOnPin(LED_PORT,LED_R);
        GPIO_setAsOutputPin(LED_PORT,LED_B);
        GPIO_setOutputLowOnPin(LED_PORT,LED_B);
        break;
    case 'Y' : //Yellow
        GPIO_setAsOutputPin(LED_PORT,LED_G);
        GPIO_setOutputLowOnPin(LED_PORT,LED_G);
        GPIO_setAsOutputPin(LED_PORT,LED_R);
        GPIO_setOutputLowOnPin(LED_PORT,LED_R);
        break;
    case 'T' : // Turkiz
        GPIO_setAsOutputPin(LED_PORT,LED_G);
        GPIO_setOutputLowOnPin(LED_PORT,LED_G);
        GPIO_setAsOutputPin(LED_PORT,LED_B);
        GPIO_setOutputLowOnPin(LED_PORT,LED_B);
        break;
    case 'W' :
        allOn();
        break;
    default :
        break;
    }

}







