/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 * ======== hal.c ========
 *
 */
#include <initBoard.h>
#include "msp430.h"

#include "driverlib.h"

#include "USB_API/USB_Common/device.h"
#include "USB_config/descriptors.h"

#include "defines.c"

#define LF_CRYSTAL_FREQUENCY_IN_HZ     32768                                    // 32KHz
#define HF_CRYSTAL_FREQUENCY_IN_HZ     12000000                                  // 12MHz

#define MCLK_DESIRED_FREQUENCY_IN_KHZ  12000                                     // 26MHz
#define MCLK_FLLREF_RATIO              MCLK_DESIRED_FREQUENCY_IN_KHZ / ( UCS_REFOCLK_FREQUENCY / 1024 )    // Ratio = 250



//#define GPIO_ALL	GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3| \
//        GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7

uint32_t myACLK;
uint32_t mySMCLK;
uint32_t myMCLK;


/*
 * This function drives all the I/O's as output-low, to avoid floating inputs
 * (which cause extra power to be consumed).  This setting is compatible with
 * TI FET target boards, the F5529 Launchpad, and F5529 Experimenters Board;  
 * but may not be compatible with custom hardware, which may have components  
 * attached to the I/Os that could be affected by these settings.  So if using
 * other boards, this function may need to be modified.
 */
void USBHAL_initPorts(void)
{
#ifdef __MSP430_HAS_PORT1_R__

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_ALL);

#endif

#ifdef __MSP430_HAS_PORT2_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT3_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT4_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT5_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT6_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT7_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT8_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORT9_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_ALL);
#endif

#ifdef __MSP430_HAS_PORTJ_R__
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_ALL);
#endif

    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_ALL);

    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_ALL);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_ALL);
    //GPIO_setOutputHighOnPin(LED_PORT,LED_R + LED_G + LED_B);
    //    // Porn down the LEDs on SPI CLONE board will need to be deleted in the functional version
    //    // Status 1
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN7);
    //    // Status 2
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0);
    //    // Status 3
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN4);
    //    // Status 4
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN5);
    //    // Status 5
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN2);
    //    // DETECT PINS
    //    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_ALL);
//GPIO_setAsOutputPin(LED_PORT,LED_R + LED_G + LED_B);
//GPIO_setDriveStrength(LED_PORT,LED_R + LED_G + LED_B,GPIO_FULL_OUTPUT_DRIVE_STRENGTH);
//
//GPIO_setOutputHighOnPin(LED_PORT,LED_R + LED_G + LED_B);

//GPIO_setAsInputPin(LED_PORT,LED_R + LED_G + LED_B);


    //    GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);
    //GPIO_setAsInputPin(LED_PORT,LED_R + LED_G + LED_B);                             // TODO Check if this is part of the alternative GPIO function
    //    GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set GPIO Pin alternative function to blink directly from timer

}

void USBHAL_initClocks(uint32_t mclkFreq)
{

    // Crystal Alternative function pins
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P5,                                // XIN  on P5.4
            GPIO_PIN3 +                                  // XT2OUT on P5.3
            GPIO_PIN2                                    // XT2IN  on P5.2
    );


    PMM_setVCore( PMM_CORE_LEVEL_3 );

    UCS_turnOnXT2( UCS_XT2_DRIVE_8MHZ_16MHZ );

    UCS_setExternalClockSource(
            LF_CRYSTAL_FREQUENCY_IN_HZ,                                         // XT1CLK input
            HF_CRYSTAL_FREQUENCY_IN_HZ                                          // XT2CLK input
    );



    UCS_initClockSignal(
            UCS_FLLREF,
            UCS_REFOCLK_SELECT,
            UCS_CLOCK_DIVIDER_1);

    // Select XT2 as oscillator source for SMCLK
    UCS_initClockSignal(
            UCS_SMCLK,                                   // Clock you're configuring
            UCS_XT2CLK_SELECT,                           // Clock source
            UCS_CLOCK_DIVIDER_1                          // Divide down clock source by this much
    );
    UCS_initClockSignal(
            UCS_MCLK,
            UCS_XT2CLK_SELECT,
            UCS_CLOCK_DIVIDER_1
    );


    UCS_initClockSignal(
            UCS_ACLK,
            UCS_REFOCLK_SELECT,
            UCS_CLOCK_DIVIDER_1);

    UCS_initFLLSettle(
            mclkFreq/1000,
            mclkFreq/32768);


}
//Released_Version_5_10_00_17
