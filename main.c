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
 * fitStatusB device
 * Andrey.tesler@compulab.co.il
 *
 * MSP430 emulates as USB CDC UART device Wit for command from the user and then
 * turn LEDS on / off depending on the commands
 * ---------------------------------------------------------------------------*/

// ****************************************************************************

/* If additional device added add code in the following functions:
targetFlashDetection()
 */

// SBC-FLT Defines


// ******************************************** INCLUDE ***********************************************
#include <initBoard.h>                                                                                  // Ti Board specific functions  // TODO remove this or add board specific
#include <string.h>                                                                                     // String library to handle the String
#include <stdio.h>                                                                                      /* printf, fgets */
#include <stdlib.h>                                                                                     //atoi
#include "driverlib.h"                                                                                  // Ti Driver library for MSP430 Devices
#include "USB_config/descriptors.h"                                                                     // USB descriptors
#include "USB_API/USB_Common/device.h"                                                                  // Part of TI USP API library
#include "USB_API/USB_Common/usb.h"                                                                     // Part of TI USP API library
#include "USB_API/USB_CDC_API/UsbCdc.h"                                                                 // Part of TI USP API library USB CDC
#include "USB_app/usbConstructs.h"                                                                      // Part of TI USP API library
#include "usbLed.h"                                                                                     // Help functions for the LED's
#include "myTimers.h"                                                                                   // Timer specific functions
#include "defines.c"                                                                                    // Global defines for the whole project


// Test Varibles
//char val;

//char ones,tens;
// ****************************************************************************************************


// ******************************************** FILE GLOBALS ******************************************
volatile uint8_t bCDCDataReceived_event = FALSE;                                                        // Flag set by event handler to indicate data has been received into USB buffer
volatile uint16_t led;                                                                                  // TODO Check what this doo

// ****************************************************************************************************


// ******************************************** FILE DEFINES ******************************************
#define BUFFER_SIZE 256
#define MAX_STR_LENGTH 128
#define INFOB_START   (0x1900)

// ****************************************************************************************************


// ******************************************** INITIALAZATION ****************************************
char dataBuffer[BUFFER_SIZE] = "";
char nl[2] = "\n";
char wholeString[MAX_STR_LENGTH] = "";                                                                  // Entire input str from last 'return'
char pieceOfString[MAX_STR_LENGTH] = "";                                                                // Holds the new addition to the string
char outString[MAX_STR_LENGTH] = "";                                                                    // Holds the outgoing string
char deviceSN[12];

// *************************************** New Flow declarations and Misc *****************************


int seqCounter = 0;
uint8_t i;                                                                                              // General counter value

uint8_t incomingColor[6];                                                                               // Array for incoming color data
uint8_t formatedColor[3];                                                                               // Formated array color data

uint8_t unformatedFade[MAX_FADE_DECIMAL] = "";                                                          // Unformatted Fade tempt storage

//uint16_t count;
//uint16_t c = 0;
//unsigned char tempR,tempR2,tempG,tempG2,tempB,tempB2 = 0;
//char buffer[2] = {0x00,0x00};
// values time fade function
//uint32_t timetoFade =300;                                                                               // Default value for time to fade
//uint8_t timeBuffer[6] = {0,0,0,0,0,0};                                                                  // Buffer to store The decimal numbers from user input
//uint32_t maxTimeToFade = 100000;                                                                        // Maximum Number to fade TODO try to remove this
//uint8_t fadeCounter = 0;                                                                                // Counter for array size
//int i=1;
int n = 0;
// ****************************************************************************************************


// ******************************************** FUNCTION DECLARATION **********************************
uint8_t retInString (char* string);
void printHelp(void);
char chrToHx(uint8_t);
void converIncomingColor(void);                                                                         // Convert incoming color to formated color
uint32_t parseFadeTimer(uint8_t unformated[],uint8_t fadeCounter);                                                                               // Convert from int to char to print current color
// ****************************************************************************************************


// ******************************************** INITIAL INITIALAZATION ********************************
//void setLeds();
// https://git-server/Andrew/fit-statUSB/wikis/Internal-Flash-Mapping                                   // Compulab Wiki page for memory mapping
// Revision information from Flash
char *MAJOR1_ptrB = (char *)INFOB_START;                                                                // Major Revision Start
char *MINOR1_ptrB = (char *)INFOB_START+2;                                                              // Minor Revision start
char *SERIAL_ptrB = (char *)INFOB_START+4;                                                              // Serial Number Start

// ****************************************************************************************************


// ******************************************** TESTING ***********************************************


//#pragma DATA_SECTION ( count, ".infoB" )

// ****************************************************************************************************


/*****************************************************************************************************
 *                                              MAIN FUNCTION
 ******************************************************************************************************/
void main (void)

{
    WDT_A_hold(WDT_A_BASE);                                                                             // Stop watchdog timer
    PMM_setVCore(PMM_CORE_LEVEL_3);                                                                     // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
    // REFTCOFF 0   REF Temp.Sensor off


    USBHAL_initPorts();           // Config GPIOS for low-power (output low)
    USBHAL_initClocks(12000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect
    /*
    // Initialize timers
    // Collect all the Device information in to one string
    //    strcat(deviceSN,"REV:");
    //    strncat(deviceSN,(char *)MAJOR1_ptrB,2);
    //    strcat(deviceSN,".");
    //    strncat(deviceSN,(char *)MINOR1_ptrB,2);
    //    strcat(deviceSN,"\r\nSerial: ");
     */

    strncat(deviceSN,(char *)SERIAL_ptrB,10);
    strcat(deviceSN,"\r\n\r\n");
    fadeTimer = 300;                                                                                    // Default timer 300ms

    __bis_SR_register( GIE );                                                                           // Enable interrupts globally


    __enable_interrupt();  // Enable interrupts globally

    // Gather information from the card
    //strcpy(deviceSN,"Serial No:\t\t\t1234567890\n\r");
    //  strcpy(deviceSN,"Device SN: 56987\t Rev.1.0\r\n\r\n");

    allOff();

    while (1)
    {
        uint8_t ReceiveError = 0, SendError = 0;
        //uint16_t count;
        //        uint8_t i;

        // Check the USB state and directly main loop accordingly
        switch (USB_getConnectionState())
        {
        // This case is executed while your device is enumerated on the
        // USB host
        case ST_ENUM_ACTIVE:



            // Sleep if there are no bytes to process.
            __disable_interrupt();
            if (!USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM)) {


                // Enter LPM0 until awakened by an event handler
                __bis_SR_register(LPM0_bits + GIE);
            }

            __enable_interrupt();

            // Exit LPM because of a data-receive event, and
            // fetch the received data
            if (bCDCDataReceived_event){

                // Add bytes in USB buffer to the string
                USBCDC_receiveDataInBuffer((uint8_t*)pieceOfString,
                                           MAX_STR_LENGTH,
                                           CDC0_INTFNUM); // Get the next piece of the string

                // Append new piece to the whole
                strcat(wholeString,pieceOfString);

                USBCDC_sendDataAndWaitTillDone((uint8_t*)pieceOfString,
                                               strlen(pieceOfString),CDC0_INTFNUM,0);

                // Echo back the characters received
                //				USBCDC_sendDataInBackground((uint8_t*)pieceOfString,
                //						strlen(pieceOfString),CDC0_INTFNUM,0);

                // Has the user pressed return yet?
                if (retInString(wholeString)){

                    // each switch case here is represents the first later in the user command
                    // And parses the string accordingly
                    switch(wholeString[0]) {
                    // Enter device USB BSL mode for program updates.
                    case 'P' : {
                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);
                        strcpy(outString,"\r\nEntering device Programming mode - Remove the device after programming\r\n\r\n");
                        // Send String here the USB will kill itself next
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        Timer_B_stop(TIMER_B0_BASE);
                        Timer_A_stop(TIMER_A0_BASE);
                        // Programming mode
                        USB_disconnect();                           // Disconnect USB device
                        __disable_interrupt();                      // Disable global interrupt
                        ((void (*)())0x1000)();                     // Set the bsl address
                        USB_connect();                              // Connect the USB back on
                        break;
                    }
                    // Set color command in the format #RRGGBB
                    // Return Value OK
                    case '#' : {
                        Timer_A_stop(TIMER_A0_BASE);                                                    // Stop Timer A0
                        Timer_B_stop(TIMER_B0_BASE);                                                    // Stop Timer B0
                        //GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set alternative mode for GPIO LEDS
                        // Store the data to array
                        for (i=0;i<=MAX_FADE_DECIMAL;i++) {                                                             // Store the unformatted data to color array
                            if (wholeString[i+1] == 0x00) {
                                break;
                            }
                            incomingColor[i] = wholeString[i+1];                                        // The first string is the switch case command

                        }
                        converIncomingColor();                                                          // Convert the unformatted data in temp array to formated decimals
                        currentRGBColor[0] = formatedColor[0];
                        currentRGBColor[1] = formatedColor[1];
                        currentRGBColor[2] = formatedColor[2];
                        initTimers(formatedColor[0],formatedColor[1],formatedColor[2]);                 // Start the timers with the formated information

                        // Test
                        strcpy(outString,"\r\nSet Color, OK \r\n");                                     // Prepare String to send
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);                  // Send the string

                        break;
                    }

                    // return fit-statUSB ID serial number as generated by USB API
                    case '?' : {
                        strcpy(outString,"\r");                                                       // Send new Line after serial number
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);

                        USBCDC_sendDataInBackground((uint8_t*)abramSerialStringDescriptor,              // Send Serial information
                                                    34,CDC0_INTFNUM,0);
                        strcpy(outString,"\r\n");                                                       // Send new Line after serial number
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;
                    }

                    // Set fading (Transition) period in ms
                    // Ftttt - default = 0300 (300 mS)
                    case 'F' : {
                        //int unformatedFadeTimer[5] = {1,11,22,2,2};
                        Timer_B_stop(TIMER_B0_BASE);                                                    // Stop Timer B0
                        int counterFade = 0;                                                            // Counter for how many actually numbers recived
                        // && wholeString[i+1] <= 9
                        for (i=0;i<MAX_FADE_DECIMAL;i++) {                                              // parse incoming text and store only the fade numbers
                            if (wholeString[i+1] >= '0'  & wholeString[i+1] <= '9' ) {
                                unformatedFade[i] = wholeString[i+1];                                   // Store the date after validating that this is ok
                                counterFade++;
                            }
                        }

                        fadeTimer = parseFadeTimer(unformatedFade,counterFade);                         // Convert the User Input to decimal numbers
                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set alternative function
                        updateFadeTime(fadeTimer);          // TODO test
                        // Restart the fade function with the designated fade time
                        //initFadeTime(fadeTimer);// TODO add function that updates the timer
                        strcpy(outString,"\r\nSet fading period, OK\r\n\r\n");                          // Prepare String for the user
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);                  // Send the String to Uart
                        break;
                    }

                    // Set color sequence B#RRGGBB#RRGGBB#RRGGBB.....
                    case 'B' : {

                        Timer_A_stop(TIMER_A0_BASE);                                                    // Stop Timer A0
                        Timer_B_stop(TIMER_B0_BASE);                                                    // Stop Timer B0
                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set alternative mode for GPIO LEDS
                        seqCounter = 0 ;                                                                // Reset Sequence color
                        //                        MAX_STR_LENGTH
                        for (i = 0 ; i < MAX_STR_LENGTH ; i ++) {                                       // Pass on the whole array of incoming data

                            if (wholeString[i] == '#'  ) {
                                for (n = 0;n <6;n++) {                                                             // Store the unformatted data to color array
                                    if (wholeString[i+1+n] == 0x00) {
                                        break;
                                    }
                                    incomingColor[n] = wholeString[i+1+n];                                        // The first string is the switch case command

                                }

                                converIncomingColor();

                                for (n = 0 ; n < 3 ; n++) {                                             // Copy converted sequence to 2D array
                                    colorSeq[seqCounter][n] = formatedColor[n];
                                }
                                if (seqCounter < MAX_SEQ_COLORS) {                                      // Check if not reached max colors
                                    seqCounter++;                                                       // Update sequence counter
                                }
                            }
                        }
                        // TODO add function that updates the timer
                        // Convert the unformatted data in temp array to formated decimals
                        //initFade();                 // Start the timers with the formated information
                        initFade(seqCounter);

                        // Test
                        strcpy(outString,"\r\nSet sequence, OK \r\n");                                     // Prepare String to send
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);


                        break;
                    }

                    // Return current color
                    case 'G' : {

                        sprintf(outString,"(%x,%x,%x)\n\r",(uint8_t)currentRGBColor[0],(uint8_t)currentRGBColor[1],(uint8_t)currentRGBColor[2]);
//                        outString[1] = outString[1] - 32;
//                        outString[2] = outString[2] - 32;
//                        outString[4] = outString[4] - 32;
//                        outString[5] = outString[5] - 32;
//                        outString[7] = outString[7] - 32;
//                        outString[8] = outString[8] - 32;

//                        strcpy(outString,(uint8_t)currentRGBColor);
                        //strcpy(outString,"\r\nSet sequence, OK \r\n");                                     // Prepare String to send
                                               USBCDC_sendDataInBackground((uint8_t*)outString,
                                                                           strlen(outString),CDC0_INTFNUM,0);
                    break;
                    }

                    // Print device info + general information on commands
                    case 'H' : {
                        printHelp();
                    }

                    default :
                        strcpy(outString,"\r\n");                                                         // Send new Line when return pressed
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;
                    }


                    // Clear the string in preparation for the next one Resolve Ghost String bug
                    for (i = 0; i < MAX_STR_LENGTH; i++){
                        wholeString[i] = 0x00;
                        outString[i]   = 0x00;
                        pieceOfString[i] = 0x00;
                        if (i < 6 ) {
                            incomingColor[i] = 0x00;                                                // reset the incoming color also
                        }
                        if (i < MAX_FADE_DECIMAL) {
                            unformatedFade[i] = 0x00;
                        }
                    }

                }


                bCDCDataReceived_event = FALSE;

            } // Data recived event
            break;

            // These cases are executed while your device is disconnected from
            // the host (meaning, not enumerated); enumerated but suspended
            // by the host, or connected to a powered hub without a USB host
            // present.
        case ST_PHYS_DISCONNECTED:
        case ST_ENUM_SUSPENDED:
        case ST_PHYS_CONNECTED_NOENUM_SUSP:
            __bis_SR_register(LPM3_bits + GIE);
            _NOP();
            break;

            // The default is executed for the momentary state
            // ST_ENUM_IN_PROGRESS.  Usually, this state only last a few
            // seconds.  Be sure not to enter LPM3 in this state; USB
            // communication is taking place here, and therefore the mode must
            // be LPM0 or active-CPU.
        case ST_ENUM_IN_PROGRESS:
        default:;
        }

        if (ReceiveError || SendError){
            // TODO: place code here to handle error
        }
    }  //while(1)
}                               // main()

/*  
 * ======== UNMI_ISR ========
 */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)
#else
#error Compiler not found!
#endif
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG ))
    {
    case SYSUNIV_NONE:
        __no_operation();
        break;
    case SYSUNIV_NMIIFG:
        __no_operation();
        break;
    case SYSUNIV_OFIFG:
        UCS_clearFaultFlag(UCS_XT2OFFG);
        UCS_clearFaultFlag(UCS_DCOFFG);
        SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
        break;
    case SYSUNIV_ACCVIFG:
        __no_operation();
        break;
    case SYSUNIV_BUSIFG:
        // If the CPU accesses USB memory while the USB module is
        // suspended, a "bus error" can occur.  This generates an NMI.  If
        // USB is automatically disconnecting in your software, set a
        // breakpoint here and see if execution hits it.  See the
        // Programmer's Guide for more information.
        SYSBERRIV = 0; // clear bus error flag
        USB_disable(); // Disable
    }
}

//Released_Version_5_10_00_17
/*
 * ======== retInString ========
 */
// This function returns true if there's an 0x0D character in the string; and if
// so, it trims the 0x0D and anything that had followed it.
uint8_t retInString (char* string)
{
    uint8_t retPos = 0,i,len;
    char tempStr[MAX_STR_LENGTH] = "";

    strncpy(tempStr,string,strlen(string));  // Make a copy of the string
    len = strlen(tempStr);

    // Find 0x0D; if not found, retPos ends up at len
    while ((tempStr[retPos] != 0x0A) && (tempStr[retPos] != 0x0D) &&
            (retPos++ < len)) ;

    // If 0x0D was actually found...
    if ((retPos < len) && (tempStr[retPos] == 0x0D)){
        for (i = 0; i < MAX_STR_LENGTH; i++){ // Empty the buffer
            string[i] = 0x00;
        }

        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);

        //...and tell the calling function that we did so
        return ( TRUE) ;

        // If 0x0D was actually found...
    } else if ((retPos < len) && (tempStr[retPos] == 0x0A)){
        // Empty the buffer
        for (i = 0; i < MAX_STR_LENGTH; i++){
            string[i] = 0x00;
        }

        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);

        //...and tell the calling function that we did so
        return ( TRUE) ;
    } else if (tempStr[retPos] == 0x0D){
        for (i = 0; i < MAX_STR_LENGTH; i++){  // Empty the buffer
            string[i] = 0x00;
        }
        // ...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);
        // ...and tell the calling function that we did so
        return ( TRUE) ;
    } else if (retPos < len){
        for (i = 0; i < MAX_STR_LENGTH; i++){  // Empty the buffer
            string[i] = 0x00;
        }

        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);

        //...and tell the calling function that we did so
        return ( TRUE) ;
    }

    return ( FALSE) ; // Otherwise, it wasn't found
}

/*
 * Function that will print the following information:
 * status		- General status of the device (SN, Date of ATP, Software revision, HW revision, Status of the LEDS)
 * Commands 	- Available command and interface with the device
 * General 		- General Information Like Copyright and disclaimer
 *
 */
void printHelp() {
    // This section will get the information for status Information()
    // SN
    // Date of ATP
    // Software Rev.
    // HW revision
    // Status of LEDS

    // Prepare the first line of print
    strcpy(outString,"*******************************************************\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    //    // add data to String of SN to test
    //    //strcat(deviceSN,"56987\t Rev.1.0\n\n\r");
    //    // Print device SN
    //    USBCDC_sendDataInBackground((uint8_t*)abramSerialStringDescriptor,              // Send Serial information
    //                                34,CDC0_INTFNUM,0);

    // Available commands, Broken to many line to simplify the look of the program
    // help command
    strcpy(outString,"P\t\t\t- Enter Device firmware update mode\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);
    // device SN
    strcpy(outString,"?\t\t\t- Return device UUID #\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"#RRGGBB\t\t\t- Set LED color according to Hex color code\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"F\t\t\t- Set Fade transition Time in ms 'F1000'\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"B\t\t\t- Set Fade transition Colors B#RRGGBB#RRGGBB#RRGGBB....\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"\t\t\t Fade transition time controlled by 'F' Command default 300ms\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"\nFirmware Revision: V0.8.1\n\n\r");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    // End of information Line
    strcpy(outString,"*******************************************************\r\n\r\n");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);


}

// TODO change to more simplified option and add support for lower case string
char chrToHx(uint8_t number) {
    char formated;
    switch (number) {
    case 48:
        formated=0x00;
        break;
    case 49:
        formated=0x01;
        break;
    case 50:
        formated=0x02;
        break;
    case 51:
        formated=0x03;
        break;
    case 52:
        formated=0x04;
        break;
    case 53:
        formated=0x05;
        break;
    case 54:
        formated=0x06;
        break;
    case 55:
        formated=0x07;
        break;
    case 56:
        formated=0x08;
        break;
    case 57:
        formated=0x09;
        break;
    case 65:
        formated=0x0A;
        break;
    case 66:
        formated=0x0B;
        break;
    case 67:
        formated=0x0C;
        break;
    case 68:
        formated=0x0D;
        break;
    case 69:
        formated=0x0E;
        break;
    case 70:
        formated=0xF;
        break;
    }
    //    formated = number - 55;
    return formated;
    // return formated+0x00;
}



//void write_infoB( uint16_t *value, uint16_t *flashLocation )
//{
//    uint16_t status;
//
//    // Erase INFOB
//    do {
//         FlashCtl_eraseSegment( (uint8_t*)INFOB_START );
//         status = FlashCtl_performEraseCheck( (uint8_t*)INFOB_START, NUMBER_OF_BYTES );
//    } while (status == STATUS_FAIL);
//
//    // Flash Write
//    FlashCtl_write16( (uint16_t*) value,
//                      (uint16_t*) flashLocation,
//                      1
//    );
//}


// Convert incoming color Asci to Decimal numbers and store in global
void converIncomingColor() {

    uint8_t tempTens,tempOnes;
    char buffer[2];

    formatedColor[0] = 0;                                                                               // Clear the array
    formatedColor[1] = 0;
    formatedColor[2] = 0;
    // Test if array place is not empty
    if (incomingColor[0] != 0x00) {
        // Convert to Hex received Red values
        sprintf(buffer,"%d", incomingColor[0]);
        tempTens = chrToHx(atol(buffer));
        sprintf(buffer,"%d", incomingColor[1]);
        tempOnes = chrToHx(atol(buffer));


        tempTens = (tempTens << 4);
        formatedColor[0] = tempTens|tempOnes;
    }

    if (incomingColor[2] != 0x00) {
        // Convert to Hex received Red values
        sprintf(buffer,"%d", incomingColor[2]);
        tempTens = chrToHx(atol(buffer));
        sprintf(buffer,"%d", incomingColor[3]);
        tempOnes = chrToHx(atol(buffer));


        tempTens = (tempTens << 4);
        formatedColor[1] = tempTens|tempOnes;

    }

    if (incomingColor[4] != 0x00) {
        // Convert to Hex received Red values
        sprintf(buffer,"%d", incomingColor[4]);
        tempTens = chrToHx(atol(buffer));
        sprintf(buffer,"%d", incomingColor[5]);
        tempOnes = chrToHx(atol(buffer));


        tempTens = (tempTens << 4);
        formatedColor[2] = tempTens|tempOnes;
    }
    // Send to the fade function
}

// Gets array of unformatted Decimal ACSCII and convert them to fade number that the program can use
uint32_t parseFadeTimer(uint8_t unformated[],uint8_t fadeCounter) {
    //    MAX_FADE_DECIMAL
    uint32_t formated = 0;
    uint32_t decimals = 1 ;                                                                             // Decimal number to calculated later
    fadeTimer = 0;

    for (i = 0 ; i < fadeCounter ; i++) {
        decimals = decimals*10;
        unformated[i] = chrToHx(unformated[i]);                                                         // Convert to decimal and place again in the array
    }

    // For loop instead of power as importing math libraries no size for this code.
    for (i = 0 ; i <= fadeCounter ; i++ ) {                                                             // For loop that calculate the time fade value and store it

        decimals = decimals/10;                                                                         // Reduce the decimal size
        formated = formated + (unformated[i]*decimals);                                                 // Multiply and store the number

    }

    //    fadeTimer = formated;
    return formated;


}
