// ----------------------------------------------------------------------------
// myTimers.c  (for lab_06c_timerHal project) 
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myTimers.h"
#include "hal.h"

void initTimers(void) {



	Timer_A_initContinuousModeParam initParam = {0};
	initParam.clockSource 				= TIMER_A_CLOCKSOURCE_SMCLK;
	initParam.clockSourceDivider		= TIMER_A_CLOCKSOURCE_DIVIDER_1;
	initParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
	initParam.timerClear				= TIMER_A_DO_CLEAR;
	initParam.startTimer				= false;
//
	Timer_A_initContinuousMode(TIMER_A0_BASE,&initParam);
//
//

	// Green
	Timer_A_initCompareModeParam initCompareParamcc1 = {0};
	initCompareParamcc1.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_1;
	initCompareParamcc1.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
	initCompareParamcc1.compareOutputMode		= TIMER_A_OUTPUTMODE_TOGGLE;
	initCompareParamcc1.compareValue			= 0x1000;
//
	// Blue
	Timer_A_initCompareModeParam initCompareParamcc2 = {0};
	initCompareParamcc2.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_2;
	initCompareParamcc2.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
	initCompareParamcc2.compareOutputMode		= TIMER_A_OUTPUTMODE_TOGGLE;
	initCompareParamcc2.compareValue			= 0xAA;

	// Red
	Timer_A_initCompareModeParam initCompareParamcc3 = {0};
	initCompareParamcc3.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_3;
	initCompareParamcc3.compareInterruptEnable	= TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
	initCompareParamcc3.compareOutputMode		= TIMER_A_OUTPUTMODE_TOGGLE;
	initCompareParamcc3.compareValue			= 0xFFFF;
//
//	Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc1);
	Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc2);
	//Timer_A_initCompareMode(TIMER_A0_BASE, &initCompareParamcc3);
//

	    Timer_A_clearTimerInterrupt( TIMER_A0_BASE );                                    // Clear TAxIFG

	    //Initiaze compare mode
	    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1 + TIMER_A_CAPTURECOMPARE_REGISTER_2 + TIMER_A_CAPTURECOMPARE_REGISTER_3);

	    Timer_A_startCounter(
	        TIMER_A0_BASE,
	        TIMER_A_CONTINUOUS_MODE
	    );
}

//*****************************************************************************
// Interrupt Service Routine
//*****************************************************************************

//#pragma vector=TIMER0_A1_VECTOR
//__interrupt void timer_ISR2 (void)
//{
//	GPIO_toggleOutputOnPin( GPIO_PORT_P4, GPIO_PIN7 );
//
//}

//#pragma vector=TIMER0_A1_VECTOR
//__interrupt void timer_ISR (void)
//
//{
//
////	GPIO_toggleOutputOnPin( GPIO_PORT_P6, GPIO_PIN0 );
//    //**************************************************************************
//    // 4. Timer ISR and vector
//    //**************************************************************************
//    switch( __even_in_range( TA0IV, TA0IV_TAIFG )) {
//     case TA0IV_NONE: break;                 // (0x00) None
//     case TA0IV_TACCR1:                      // (0x02) CCR1 IFG
//    	 GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
////    	 GPIO_toggleOutputOnPin( GPIO_PORT_P6, GPIO_PIN0 );
////          _no_operation();
//           break;
//     case TA0IV_TACCR2:                      // (0x04) CCR2 IFG
//    	 GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//
//
////          _no_operation();
//           break;
//     case TA0IV_TACCR3:                      // (0x06) CCR3 IFG
//
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

