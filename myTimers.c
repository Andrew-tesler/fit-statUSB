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
uint8_t justCounter;
uint8_t smallerCounter;
uint8_t colorsNumber;                                                                   // Number of colors transition
//
//int colorCounter[MAX_SEQ_COLORS][3];                                                    // Counter for fade logic
uint32_t colorLocation;                                                        // Which color currently fading for fade logic
uint8_t direction = 0;
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

    uint8_t Red,Green,Blue;

    // Test for value correctness
    if (red >= 0 & red <= 255)
        Red = red;
    if (green >= 0 & green <= 255)
        Green = green;
    if (blue >= 0 & blue <= 255)
        Blue = blue;

    //    currentRGBColor[0] = Red;
    //    currentRGBColor[1] = Green;
    //    currentRGBColor[2] = Blue;
    GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);
    //
    //	Timer_A_initUpDownModeParam initUpDownParam = {0};
    //	initUpDownParam.clockSource								= TIMER_A_CLOCKSOURCE_SMCLK;
    //	initUpDownParam.clockSourceDivider						= TIMER_A_CLOCKSOURCE_DIVIDER_1;
    //	initUpDownParam.timerInterruptEnable_TAIE				= TIMER_A_TAIE_INTERRUPT_DISABLE;
    //	initUpDownParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    //	initUpDownParam.timerPeriod 							= 0xFF;
    //	initUpDownParam.timerClear								= TIMER_A_DO_CLEAR;
    //	initUpDownParam.startTimer								= false;





    Timer_A_initUpModeParam initParam2 = {0};
    //
    initParam2.clockSource				= TIMER_A_CLOCKSOURCE_SMCLK;
    initParam2.clockSourceDivider		= TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initParam2.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initParam2.captureCompareInterruptEnable_CCR0_CCIE	= TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initParam2.timerPeriod				= 0xFF;
    initParam2.timerClear				= TIMER_A_DO_CLEAR;
    initParam2.startTimer				= false;

    //	Timer_A_initContinuousModeParam initParam = {0};
    //	initParam.clockSource 				= TIMER_A_CLOCKSOURCE_SMCLK;
    //	initParam.clockSourceDivider		= TIMER_A_CLOCKSOURCE_DIVIDER_1;
    //	initParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    //	initParam.timerClear				= TIMER_A_DO_CLEAR;
    //	initParam.startTimer				= true;


    //		Timer_A_initUpDownMode(TIMER_A0_BASE,&initUpDownParam);

    Timer_A_initUpMode(TIMER_A0_BASE,&initParam2);

    //	Timer_A_initContinuousMode(TIMER_A0_BASE,&initParam);

    //TIMER_A_OUTPUTMODE_SET_RESET  // fitstatusb
    //TIMER_A_OUTPUTMODE_TOGGLE_SET // ARDUMSP

    // Green
    Timer_A_initCompareModeParam initCompareParamcc1 = {0};
    initCompareParamcc1.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_1;
    initCompareParamcc1.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompareParamcc1.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    //    initCompareParamcc1.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
    initCompareParamcc1.compareValue			= Red;
    //
    // Blue
    Timer_A_initCompareModeParam initCompareParamcc2 = {0};
    initCompareParamcc2.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_2;
    initCompareParamcc2.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompareParamcc2.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    //    initCompareParamcc2.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
    initCompareParamcc2.compareValue			= Blue;

    // Red
    Timer_A_initCompareModeParam initCompareParamcc3 = {0};
    initCompareParamcc3.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_3;
    initCompareParamcc3.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initCompareParamcc3.compareOutputMode       = TIMER_A_OUTPUTMODE_TOGGLE_SET;    // ARDUMSP
    //    initCompareParamcc3.compareOutputMode       = TIMER_A_OUTPUTMODE_SET_RESET;     // FITSTATUSB
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


    if (Red == 0) {
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, LED_R); /// Input power off the RED led
    }





    //
    //	    Timer_A_startCounter(
    //	        TIMER_A0_BASE,
    //	        TIMER_A_CONTINUOUS_MODE
    //	    );
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

    //    fadeTime = totalTime/fadeColorStep;                                         // Calculate fade step


    // initfade(0,0,0,0,0,0,0);
    fadeTimer = totalTime;
    initFade(colorsNumber);
}

// Get the colors sequence and sequence number and initialize the fade command
void initFade(uint8_t colorNum) {
    // Calculate the difference between each fade sequence
    //    for (justCounter = 0 ; justCounter < colorNum-1 ; justCounter++) {
    //        for (smallerCounter = 0 ; smallerCounter < 3 ; smallerCounter++) {
    //            colorFadeDiff[justCounter][smallerCounter] = colorSeq[justCounter+1][smallerCounter] - colorSeq[justCounter][smallerCounter];   // Calculate and store the difference
    //            fadeTimeMs[justCounter][smallerCounter] = fadeTimer / (double)colorFadeDiff[justCounter][smallerCounter];                       // Calculate and store the time parameter to fade
    //        }
    //    }

    colorsNumber = colorNum;
    colortick = 0;                                                                                                                          // Clear time tick number

    initTimers(colorSeq[0][0], colorSeq[0][1], colorSeq[0][2]);                                                                             // Start the LED with the first color
    colorLocation = 0;

    for (justCounter = 0 ; justCounter < MAX_SEQ_COLORS ; justCounter++) {
        for (smallerCounter = 0 ; smallerCounter < 3 ; smallerCounter++) {

            colorFadeDiff[justCounter][smallerCounter] = (colorSeq[justCounter+1][smallerCounter] - colorSeq[justCounter][smallerCounter]) / fadeTimer;

        }
    }

    initfadeClock();                                                                                                                        // Start Timer B0
}

// Function to be called by TIMER0_B0 that will update the current color according to calculated values
void updateFadeColor(){
    Timer_A_stop(TIMER_A0_BASE);// Stop Timer


    switch (direction) {
    case 0: {
        currentRGBColor[0] =   currentRGBColor[0] + colorFadeDiff[0][0];
        currentRGBColor[1] =   currentRGBColor[1] + colorFadeDiff[0][1];
        currentRGBColor[2] =   currentRGBColor[2] + colorFadeDiff[0][2];
        break;
    }

    case 1: {
        currentRGBColor[0] =   currentRGBColor[0] - colorFadeDiff[0][0];
        currentRGBColor[1] =   currentRGBColor[1] - colorFadeDiff[0][1];
        currentRGBColor[2] =   currentRGBColor[2] - colorFadeDiff[0][2];
        break;
    }
    }




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
    //colortick++;
    //    colortick[1]++;
    //    colortick[2]++;
    updateFadeColor();
    colorLocation++;

    if (colorLocation == fadeTimer){
        direction = !direction;
        colorLocation=0;
    }
    // Calculate only when needed  TODO add code later






    //initTimers(colorSeq[colortick][0], colorSeq[0][1], colorSeq[0][2]);
    //initTimers(currentRGBColor[0] + colorFadeDiff[0] ,currentRGBColor[1] + colorFadeDiff[1],currentRGBColor[2] + colorFadeDiff[2]);
    //    initTimers(currentRGBColor[0],currentRGBColor[1],currentRGBColor[2]);
    //colorLocation // Location of current color
    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);



    /*

    for (smallerCounter = 0 ; smallerCounter < 3 ; smallerCounter++) {
        if (colorFadeDiff[0][smallerCounter] > 0) {
            fadeDirection[smallerCounter] = 1;  // Count up
        }
        else {
            fadeDirection[smallerCounter] = 0; // Count down
        }


        switch (fadeDirection[smallerCounter]) {

        case 1: {
            if (colortick[smallerCounter] >= fadeTimeMs[0][smallerCounter]) {
                currentRGBColor[smallerCounter]++;
                colortick[smallerCounter] = 0;
            }


        }
        case 0: {
            if (colortick[smallerCounter] >= fadeTimeMs[0][smallerCounter]) {
                currentRGBColor[smallerCounter] = currentRGBColor[smallerCounter]-1;
                colortick[smallerCounter] = 0;
            }
        }

        }


    }
    Timer_A_stop(TIMER_A0_BASE);
    initTimers(currentRGBColor[0],currentRGBColor[1],currentRGBColor[2]);
     */

    //    initTimers(currentRGBColor[0],currentRGBColor[1],currentRGBColor[2]);
    //
    //    if (timerTick >= 100) {
    //        currentRGBColor[0] = currentRGBColor[0] + 1 ;
    //        timerTick=0;
    //        Timer_A_stop(TIMER_A0_BASE);
    //        initTimers(currentRGBColor[0],currentRGBColor[1],currentRGBColor[2]);
    //    }
    // if ()
    //    colorCounter[MAX_SEQ_COLORS][3];
    //    currentRGBColor
    //    fadeTimeMs
    //    colorSeq


    // Stop Timer A and update only when needed




    /*


    //GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set GPIO Pin alternative function to blink directly from timer
    initTimers(0x01,0x01,timer);




    //
    //
    ////
    //    fadeTickGlobal = fadeTime/2;                                                  // TODO the 255 number will have to be the color diference of each led

    if (fadeTick <= fadeTimer) {                                                           // If the fade tick smaller that the calculted global
        //
        fadeTick++;

    }
    ////    // FasdeTick havent reached incriment the counter
    else {
        fadeTick=0;

        switch(direction) {
        // Counting UP
        case 0:
            if (timer<fadeColorStep) {
                timer++;
            }
            else {
                direction=1;
            }
            break;
            // Counting Down
        case 1:
            if (timer>0) {
                timer--;
            }
            else {
                direction=0;
            }

            break;
        }
        //
    }
    ////
    //                                                                 // Update the timer


    //
    ////    GPIO_setAsOutputPin(LED_PORT, LED_B);
    ////        GPIO_toggleOutputOnPin( LED_PORT, LED_B );
    //
    //
     *
     */


}
//
//#pragma vector=TIMER0_B1_VECTOR
//__interrupt void timer_ISRB1 (void) {
//    //GPIO_toggleOutputOnPin( LED_PORT, LED_G );
//    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_1);
//
//
//}

//#pragma vector=TIMER0_A1_VECTOR
//__interrupt void timer_ISR2 (void)
//{
//    GPIO_toggleOutputOnPin( GPIO_PORT_P4, GPIO_PIN7 );
//
//}
//
//#pragma vector=TIMER0_A0_VECTOR
//__interrupt void TIMER1_A0_ISR (void)
//
//{
//
//    //GPIO_toggleOutputOnPin( GPIO_PORT_P6, GPIO_PIN0 );
//    //**************************************************************************
//    // 4. Timer ISR and vector
//    //**************************************************************************
//    switch( __even_in_range( TA0IV, TA0IV_TAIFG )) {
//    case TA0IV_NONE: break;                 // (0x00) None
//    case TA0IV_TACCR1:                      // (0x02) CCR1 IFG
//        // GPIO_toggleOutputOnPin(LED_PORT, LED_G);
//        //    	 GPIO_toggleOutputOnPin( GPIO_PORT_P6, GPIO_PIN0 );
//        //          _no_operation();
//        break;
//    case TA0IV_TACCR2:                      // (0x04) CCR2 IFG
//        //    	 GPIO_setOutputHighOnPin( LED_PORT, LED_R );
//        // GPIO_toggleOutputOnPin( LED_PORT, LED_R );
//
//        //          _no_operation();
//        break;
//    case TA0IV_TACCR3:                      // (0x06) CCR3 IFG
//
//        //          _no_operation();
//        break;
//    case TA0IV_TACCR4:                      // (0x08) CCR4 IFG
//        _no_operation();
//        break;
//    case TA0IV_5: break;                    // (0x0A) Reserved
//    case TA0IV_6: break;                    // (0x0C) Reserved
//    case TA0IV_TAIFG:                       // (0x0E) TA0IFG - TAR overflow
//        // Toggle LED2 (Green) LED on/off
//
//        _no_operation();
//        break;
//    default: _never_executed();
//    }
//}

////***** Defines ***************************************************************
//
//
////***** Global Variables ******************************************************
//
//
////*****************************************************************************
//// Initialize Timer
////*****************************************************************************
//void initTimers(void)
//{
//    //*************************************************************************
//    // 1. Setup Timer (TAR, TACTL)
//    //    TimerA in Up mode using ACLK
//    //    Toggle LED1 (Red) on/off every second directly from the timers CCR2 output pin
//    //    Toggle LED2 (Green) on/off every second using TAxIFG
//    //*************************************************************************
//    Timer_A_initUpModeParam initUpParam = { 0 };
//        initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;                       // Use ACLK (slower clock)
//        initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;           // Input clock = ACLK / 1 = 32KHz
//        initUpParam.timerPeriod = 0xFFFF / 2;                                     // Period (0xFFFF/2):  8000 / 32Khz = 1 second
//        initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;    // Enable TAR -> 0 interrupt
//        initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
//            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;                                  // No CCR0 interrupt
//        initUpParam.timerClear = TIMER_A_DO_CLEAR;                                // Clear TAR & clock divider
//        initUpParam.startTimer = false;                                           // Don't start the timer, yet
//    Timer_A_initUpMode( TIMER_A0_BASE, &initUpParam );
//
//    //*************************************************************************
//    // 2. Setup Capture & Compare features
//    //*************************************************************************
//    Timer_A_initCompareModeParam initCcr2Param = { 0 };
//        initCcr2Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;        // Use CCR2 for compare
//        initCcr2Param.compareInterruptEnable =
//            TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;                             // Since directly driving LED, interrup not req'd
//        initCcr2Param.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_RESET;        // Toggle provides a 1 sec period based on CCR0 and CCR2 values
//        initCcr2Param.compareValue = 0x100;                                      // Compare value: 4000 = 1/2 second
//    Timer_A_initCompareMode( TIMER_A0_BASE, &initCcr2Param );
//
//
//
//    //*************************************************************************
//    // 3. Clear/enable interrupt flags and start timer
//    //*************************************************************************
//    Timer_A_clearTimerInterrupt( TIMER_A0_BASE );                                    // Clear TAxIFG
//
//    //Initiaze compare mode
//    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2 + TIMER_A_CAPTURECOMPARE_REGISTER_3);
//
//
//
//
//    Timer_A_startCounter(
//        TIMER_A0_BASE,
//        TIMER_A_UP_MODE
//    );
//}
//
////*****************************************************************************
//// Interrupt Service Routine
////*****************************************************************************
//#pragma vector=TIMER0_A0_VECTOR
//__interrupt void timer_ISR (void)
//{
//    //**************************************************************************
//    // 4. Timer ISR and vector
//    //**************************************************************************
//    switch( __even_in_range( TA0IV, TA0IV_TAIFG )) {
//     case TA0IV_NONE: break;                 // (0x00) None
//     case TA0IV_TACCR1:                      // (0x02) CCR1 IFG
//          _no_operation();
//           break;
//     case TA0IV_TACCR2:                      // (0x04) CCR2 IFG
//    	 GPIO_toggleOutputOnPin( GPIO_PORT_P4, GPIO_PIN7 );
////          _no_operation();
//           break;
//     case TA0IV_TACCR3:                      // (0x06) CCR3 IFG
//    	 GPIO_toggleOutputOnPin( GPIO_PORT_P6, GPIO_PIN0 );
////          _no_operation();
//           break;
//     case TA0IV_TACCR4:                      // (0x08) CCR4 IFG
//          _no_operation();
//           break;
//     case TA0IV_5: break;                    // (0x0A) Reserved
//     case TA0IV_6: break;                    // (0x0C) Reserved
//     case TA0IV_TAIFG:                       // (0x0E) TA0IFG - TAR overflow
//          // Toggle LED2 (Green) LED on/off
//
//    	 _no_operation();
//          break;
//     default: _never_executed();
//    }
//}

