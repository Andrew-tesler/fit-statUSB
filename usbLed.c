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
#include "defines.c"

// For debugging if defined ARDUMSP some of the board GPIO will change direction to correspond with Ardumsp board that is used for initial code implimitation


/**
 * turn off all of the LED's
 */
void allOff() {


#ifdef ARDUMSP
    GPIO_setAsOutputPin(LED_PORT,LED_R);
    GPIO_setAsOutputPin(LED_PORT,LED_G);
    GPIO_setAsOutputPin(LED_PORT,LED_B);
    GPIO_setOutputLowOnPin(LED_PORT,LED_R);
    GPIO_setOutputLowOnPin(LED_PORT,LED_G);
    GPIO_setOutputLowOnPin(LED_PORT,LED_B);
#else
GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_ALL); /// Input power off the RED led




#endif //ARDUMSP


}

/**
 * Turn on all of the LED's
 */
void allOn() {
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


void ledOn(char led) {
    allOff();                                                       // Initially turn off all of the LEDs

    switch(led) {
    case 'R' :
        GPIO_setAsOutputPin(LED_PORT,LED_R);

        #ifdef ARDUMSP
            GPIO_setOutputHighOnPin(LED_PORT,LED_R);
        #else
            GPIO_setOutputLowOnPin(LED_PORT,LED_R);
        #endif //ARDUMSP
        break;

    case 'G' :
        GPIO_setAsOutputPin(LED_PORT,LED_G);
        #ifdef ARDUMSP
            GPIO_setOutputHighOnPin(LED_PORT,LED_G);
        #else
            GPIO_setOutputLowOnPin(LED_PORT,LED_G);
        #endif //ARDUMSP
        break;

    case 'B' :
        GPIO_setAsOutputPin(LED_PORT,LED_B);
        #ifdef ARDUMSP
            GPIO_setOutputHighOnPin(LED_PORT,LED_B);
        #else
            GPIO_setOutputLowOnPin(LED_PORT,LED_B);
        #endif //ARDUMSP
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







