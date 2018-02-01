// ----------------------------------------------------------------------------
// myTimers.c  (for lab_06c_timerHal project) 
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include <initBoard.h>
#include "myTimers.h"
#include "defines.c"


//char timer = 0x00;
//int direction = 0;                                                                      // Direction of the fade
//
//unsigned int timerBcounter[3];                                                          // Counter for each of the colors
//
int justCounter;                                                            // Counter for the for loop
int smallerCounter;                                                         // Counter for the dor loop used for RGB iterration
int colorsNumber;                                                           // Number of colors transition
//
//int colorCounter[MAX_SEQ_COLORS][3];                                          // Counter for fade logic

int test;
int fadeArrayLocation;                                                      // Location in fadeArrayDiff array
//uint8_t fadeArrayDirection;                                                     // Probably not needed
// Counter Tick for fade logic
//uint8_t fadeDirection[3];                                                               // Direction of LED to fade

//uint8_t timerTick = 0;
//uint16_t fadeTick = 0;              // Fade tick - Temp Value for the timer function, Each clock increment increments this value
//uint16_t fadeTickGlobal=5000;       // fadeTickGlobal - Global time from color transition from one color to another in ms
//uint16_t fadeTime = 0;              // Fade time - calculated value from the global time needed and the steps to transition
//uint8_t fadeColorStep = 255;          // Steps for transition to the next desired color
// Fade time between each fade intervals TODO Will need to use with color differential number
//Total time of fade from one color to another

void initTimers(int red,int green,int blue) {

    uint8_t Red,Green,Blue;                                                     // Initialize RGB Int's
    GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);// Set the RGB LED GPIO to alternative function to power the LEDS directly from timer
    // Test for value correctness
    if (red > 0 & red <= 255){                                                  // Check for Correct color numbers and pass only them
        Red = red;
    }
    else {
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, LED_R);
    }
    if (green > 0 & green <= 255){
        Green = green;
    }
    else {
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, LED_G);
    }
    if (blue > 0 & blue <= 255){
        Blue = blue;
    }
    else {
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, LED_B);
    }

    //    if (Red <= 0) {                                                             // Fix the LED red Powering ON when not in use
    //        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, LED_R);     // Input power off the RED led
    //    }





    Timer_A_initUpModeParam initParam2 = {0};
    //
    initParam2.clockSource				= TIMER_A_CLOCKSOURCE_SMCLK;
    initParam2.clockSourceDivider		= TIMER_A_CLOCKSOURCE_DIVIDER_24;
    initParam2.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initParam2.captureCompareInterruptEnable_CCR0_CCIE	= TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initParam2.timerPeriod				= 0xFF;
    initParam2.timerClear				= TIMER_A_DO_CLEAR;
    initParam2.startTimer				= false;


    Timer_A_initUpMode(TIMER_A0_BASE,&initParam2);

    // Green
    Timer_A_initCompareModeParam initCompareParamcc1 = {0};
    initCompareParamcc1.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_1;
    initCompareParamcc1.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    //    initCompareParamcc1.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    initCompareParamcc1.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
    initCompareParamcc1.compareValue			= Red;
    //
    // Blue
    Timer_A_initCompareModeParam initCompareParamcc2 = {0};
    initCompareParamcc2.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_2;
    initCompareParamcc2.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    //    initCompareParamcc2.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    initCompareParamcc2.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
    initCompareParamcc2.compareValue			= Blue;

    // Red
    Timer_A_initCompareModeParam initCompareParamcc3 = {0};
    initCompareParamcc3.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_3;
    initCompareParamcc3.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    //    initCompareParamcc3.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    initCompareParamcc3.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
    initCompareParamcc3.compareValue			= Green;
    //
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc1);
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc2);
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc3);
    //

    Timer_A_clearTimerInterrupt( TIMER_A0_BASE );                                    // Clear TAxIFG

    //    GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);
    //Initiaze compare mode
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1 + TIMER_A_CAPTURECOMPARE_REGISTER_2 + TIMER_A_CAPTURECOMPARE_REGISTER_3);

    //Timer_A_disableInterrupt(TIMER_A0_BASE);

    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
}

// Fade the leds from given color to given color by the time given
void initfadeClock() {

    Timer_B_initUpModeParam initTimer_b = {0};
    //    0xDCD;
    initTimer_b.clockSource                             = TIMER_B_CLOCKSOURCE_SMCLK;
    initTimer_b.clockSourceDivider                      = TIMER_B_CLOCKSOURCE_DIVIDER_7;
    initTimer_b.timerPeriod                             = 0xDCD;
    initTimer_b.timerInterruptEnable_TBIE               = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initTimer_b.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    initTimer_b.timerClear                              = TIMER_B_DO_CLEAR;
    initTimer_b.startTimer                              = false;

    Timer_B_initUpMode(TIMER_B0_BASE, &initTimer_b);

    Timer_B_startCounter(TIMER_B0_BASE,TIMER_B_UP_MODE);

}


// calculate  the time for the fade function
void updateFadeTime(uint32_t totalTime) {

    fadeTimer = totalTime;
    initFade(colorsNumber);
}

// Get the colors sequence and sequence number and initialize the fade command
void initFade(uint8_t colorNum) {
    colorsNumber = colorNum;
    colortick = 0;                                                                                                                          // Clear time tick number
    fadeArrayLocation = 0;
    direction = 0;
    //    fadeArrayDirection = 0;
    initTimers(colorSeq[0][0], colorSeq[0][1], colorSeq[0][2]);                                                                             // Start the LED with the first color
    colorLocation = 0;
    currentRGBColor[0] = colorSeq[0][0];
    currentRGBColor[1] = colorSeq[0][1];
    currentRGBColor[2] = colorSeq[0][2];
//    colorFadeTimer = 0;

    // Debug
    //colorFadeTimer[0] = 1000;
    //    colorFadeTimer[1] = 200;;
    //    colorFadeTimer[2] = 3000;
    //    colorFadeTimer[3] = 20;



    for (justCounter = 0 ; justCounter < colorsNumber-1 ; justCounter++) {                                                                    // The last run of the loop calculates the time from the last element to first element
        for (smallerCounter = 0 ; smallerCounter < 3 ; smallerCounter++) {                                                                  // Each R,G,B color calculation


            //            colorFadeDiff[justCounter][smallerCounter] = (colorSeq[justCounter+1][smallerCounter] - colorSeq[justCounter][smallerCounter]) / colorFadeTimer[justCounter];
            colorFadeDiff[justCounter][smallerCounter] = (colorSeq[justCounter+1][smallerCounter] - colorSeq[justCounter][smallerCounter]);
        }
    }
    // Calculate the last transition
    for (smallerCounter = 0 ; smallerCounter < 3 ; smallerCounter++) {

        //        colorFadeDiff[colorsNumber-1][smallerCounter] = (colorSeq[0][smallerCounter] - colorSeq[colorsNumber-1][smallerCounter]) / colorFadeTimer[colorsNumber-1];
        colorFadeDiff[colorsNumber-1][smallerCounter] = (colorSeq[0][smallerCounter] - colorSeq[colorsNumber-1][smallerCounter]);
    }
    initfadeClock();                                                                                                                        // Start Timer_B0
}

// Function to be called by TIMER0_B0 that will update the current color according to calculated values
void updateFadeColor(){
    Timer_A_stop(TIMER_A0_BASE);// Stop Timer


    //    switch (direction) {
    //    case 0: {
    //        currentRGBColor[0] =   currentRGBColor[0] + colorFadeDiff[fadeArrayLocation][0];
    //        currentRGBColor[1] =   currentRGBColor[1] + colorFadeDiff[fadeArrayLocation][1];
    //        currentRGBColor[2] =   currentRGBColor[2] + colorFadeDiff[fadeArrayLocation][2];
    //        break;
    //    }
    //
    //    case 1: {
    //        currentRGBColor[0] =   currentRGBColor[0] - colorFadeDiff[fadeArrayLocation][0];
    //        currentRGBColor[1] =   currentRGBColor[1] - colorFadeDiff[fadeArrayLocation][1];
    //        currentRGBColor[2] =   currentRGBColor[2] - colorFadeDiff[fadeArrayLocation][2];
    //        break;
    //    }
    //    }


    //    currentRGBColor[0] =   currentRGBColor[0] + colorFadeDiff[fadeArrayLocation][0];
    //    currentRGBColor[1] =   currentRGBColor[1] + colorFadeDiff[fadeArrayLocation][1];
    //    currentRGBColor[2] =   currentRGBColor[2] + colorFadeDiff[fadeArrayLocation][2];
//    currentRGBColor[0] =   currentRGBColor[0] + colorFadeDiff[fadeArrayLocation][0];
//    currentRGBColor[1] =   currentRGBColor[1] + colorFadeDiff[fadeArrayLocation][1];
//    currentRGBColor[2] =   currentRGBColor[2] + colorFadeDiff[fadeArrayLocation][2];


    initTimers(currentRGBColor[0],currentRGBColor[1],currentRGBColor[2]);


}
//*****************************************************************************
// Interrupt Service Routine
//*****************************************************************************
//
#pragma vector=TIMER0_B0_VECTOR
__interrupt void timer_ISRB0 (void) {
    // the timer should run and update the PWM clock(TIMERA0) if any change needed
    // Because of several fade values present the script will also need to iterate on the array of colors


    // colorFadeDiff -- Fade diff array location
    // colorLocation -- Color location in each of the fade diff array colors
    // direction     -- Direction of the fade   // colorsNumber  -- Total number of array size of fade diff


    currentRGBColor[0] =   ((colorLocation * colorFadeDiff[fadeArrayLocation][0]) /  colorFadeTimer[fadeArrayLocation]) + colorSeq[fadeArrayLocation][0];
    currentRGBColor[1] =   ((colorLocation * colorFadeDiff[fadeArrayLocation][1]) /  colorFadeTimer[fadeArrayLocation]) + colorSeq[fadeArrayLocation][1];
    currentRGBColor[2] =   ((colorLocation * colorFadeDiff[fadeArrayLocation][2]) /  colorFadeTimer[fadeArrayLocation]) + colorSeq[fadeArrayLocation][2];


    updateFadeColor();



    colorLocation++;
//
//
    if (colorLocation >= colorFadeTimer[fadeArrayLocation]){   // Test if the array of the color reached its limits
//
        if (disableDirection == 1) {
            Timer_B_stop(TIMER_B0_BASE);
        }

        if (fadeArrayLocation == colorsNumber-1) {
            fadeArrayLocation = 0;
            //colorLocation=0;
        }
        else if (fadeArrayLocation < colorsNumber-1){
            fadeArrayLocation++;
        }
//        //        fadeArrayLocation++;
//        //        fadeArrayLocation %= colorsNumber; // return fadeArraylocation to zero if reached colorsNumber
//
        colorLocation=0;


        //        switch (direction) {
        //
        //        case 0: {   // Move forward
        //            if (fadeArrayLocation == colorsNumber-2) {
        //                direction = !direction;
        //                //colorLocation=0;
        //            }
        //            else if (fadeArrayLocation < colorsNumber-2){
        //                fadeArrayLocation++;
        //            }
        //
        //            colorLocation=0;
        //            break;
        //        }
        //        case 1: {  // Move backward
        //            if (fadeArrayLocation == 0) {
        //                direction = !direction;
        //                //colorLocation=0;
        //            }
        //            else if (fadeArrayLocation > 0) {
        //                fadeArrayLocation--;
        //            }
        //            colorLocation=0;
        //            break;
        //        }
        //
        //        }

    }



    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);
}
