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
char val;

char ones,tens;
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
char deviceSN[128];
uint16_t count;
uint16_t c = 0;
unsigned char tempR,tempR2,tempG,tempG2,tempB,tempB2 = 0;
char buffer[2] = {0x00,0x00};

// values time fade function
uint32_t timetoFade =300;                                                                               // Default value for time to fade
uint8_t timeBuffer[6] = {0,0,0,0,0,0};                                                                  // Buffer to store The decimal numbers from user input
uint32_t maxTimeToFade = 100000;                                                                        // Maximum Number to fade TODO try to remove this
uint8_t fadeCounter = 0;                                                                                // Counter for array size

int i=1;
int n = 0;
// ****************************************************************************************************


// ******************************************** FUNCTION DECLARATION **********************************
uint8_t retInString (char* string);
void printHelp(void);
char chrToHx(uint8_t);

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
    // Initialize timers

    // Collect all the Device information in to one string
    strcat(deviceSN,"REV:");
    strncat(deviceSN,(char *)MAJOR1_ptrB,2);
    strcat(deviceSN,".");
    strncat(deviceSN,(char *)MINOR1_ptrB,2);
    strcat(deviceSN,"\r\nSerial: ");
    strncat(deviceSN,(char *)SERIAL_ptrB,10);
    strcat(deviceSN,"\r\n\r\n");


    __bis_SR_register( GIE );                                                   // Enable interrupts globally


    __enable_interrupt();  // Enable interrupts globally

    // Gather information from the card
    //strcpy(deviceSN,"Serial No:\t\t\t1234567890\n\r");
  //  strcpy(deviceSN,"Device SN: 56987\t Rev.1.0\r\n\r\n");

    allOff();

    while (1)
    {
        uint8_t ReceiveError = 0, SendError = 0;
        //uint16_t count;
        uint8_t i;

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

                    switch(wholeString[0]) {
                    case '@' :                                                                          // Set The led color based on String argument TODO Remove in final version
                        Timer_A_stop(TIMER_A0_BASE);
                        Timer_B_stop(TIMER_B0_BASE);
                        allOff();
//                        ledOn(wholeString[1]);                                                          // Function to set the LEDs
                        strcpy(outString,"\r\nLED is ON--Suspended function\r\n\r\n");                                      // Prepare String for Console print

                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);                  // Send the String to USB Console
                        break;

                    case '#' :                                                                          // Set Led color #RRGGBB
                        Timer_A_stop(TIMER_A0_BASE);
                        Timer_B_stop(TIMER_B0_BASE);
                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set GPIO Pin alternative function to blink directly from timer
                        // Convert to Hex received Red values
                        sprintf(buffer,"%d", wholeString[1]);
                        tens = chrToHx(atol(buffer));
                        sprintf(buffer,"%d", wholeString[2]);
                        ones = chrToHx(atol(buffer));


                        tens = (tens << 4);
                        tempR = tens|ones;

                        // Convert to HEX received Green values
                        sprintf(buffer, "%d", wholeString[3]);
                        tens = chrToHx(atol(buffer));
                        sprintf(buffer,"%d", wholeString[4]);
                        ones = chrToHx(atol(buffer));

                        tens = (tens << 4);
                        tempG = tens|ones;

                        // Convert to HEX received Blue values
                        sprintf(buffer, "%d", wholeString[5]);
                        tens = chrToHx(atol(buffer));
                        sprintf(buffer,"%d", wholeString[6]);
                        ones = chrToHx(atol(buffer));

                        tens = (tens << 4);
                        tempB = tens|ones;

                        // Send to the fade function
                        initTimers(tempR,tempG,tempB);

                        strcpy(outString,"\r\nPressed Fade Command\r\n\r\n");
                        // Send the String to USB
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;

                    case 'B' :
                        Timer_A_stop(TIMER_A0_BASE);
                        Timer_B_stop(TIMER_B0_BASE);
                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set GPIO Pin alternative function to blink directly from timer
                        allOff();
                        initfade(0,0,0,0,0,0,0);
                        // Set color sequence transition from one color to another issue #8
                        strcpy(outString,"\r\n… Set color sequence\r\n\r\n");
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                                            strlen(outString),CDC0_INTFNUM,0);
                        break;

                    case '?' :
                        // Return fit-statUSB ID
                        // Return the Serial number of the device
                        // For now return ATP-programmed number.
                        // TODO Change the serial to return only Serial number without all of the REV string....
                        USBCDC_sendDataInBackground((uint8_t*)deviceSN,
                                                                                strlen(deviceSN),CDC0_INTFNUM,0);
                        break;


                    case 'F':
                        // Set fading (transition) period in ms
                        // TODO Add global value that will serve as the global value for the transition time when set color sequence is called
                        Timer_B_stop(TIMER_B0_BASE);                                                    // Stop timer
                        // Find the Decimal number to fade in the Buffer
                        timetoFade=0;                                                                   // Initialize time to fade value
                        maxTimeToFade = 1;                                                              // Initialize Max time to fade
                        fadeCounter=0;                                                                  // Initialize fade counter
                        // Max 6 Digits
                        for (i=0;i <= 6 ; i++) {                                                        // For loop that extract decimals from user input
                            if (wholeString[i+1] >= '0' && wholeString[i+1] <= '9') {
                                fadeCounter++;                                                          // Increment counter to be used in the next for loop
                                timeBuffer[i] = chrToHx(wholeString[i+1]);                              // Store to buffer the converted decimal number
                                maxTimeToFade = maxTimeToFade*10;                                       // Increment the max time to fade decimal places
                            }
                        }
                        for (i = 0 ; i <= fadeCounter ; i++ ) {                                         // For loop that calculate the time fade value and store it
                            maxTimeToFade = maxTimeToFade/10;                                           // Multiply the timer digits
                            timetoFade = timetoFade + (timeBuffer[i]*maxTimeToFade);
                        }

                        GPIO_setAsPeripheralModuleFunctionOutputPin(LED_PORT,LED_R + LED_G + LED_B);    // Set alternative function
                        GPIO_setAsInputPin(LED_PORT, LED_R);                                            // TODO disabled Red LED for debuging
                        initFadeTime(timetoFade);
                        strcpy(outString,"\r\nSet fading period\r\n\r\n");
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;


                    case 'G':
                        // get current color
                        // TODO should be some value that is stored in the internal device Flash
                        // Returns print #RRGGBB

                        strcpy(outString,"\r\nDevice color #RRGGBB\r\n\r\n");
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;

                    case '-' :

                        if (wholeString[1] == 'P') {
                            strcpy(outString,"\r\nPrograming mode\r\n\r\n");
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

                        }
                        /// Test If for String outputs // TODO remove in final version
                        else if (wholeString[1] =='T') {

                            USBCDC_sendDataInBackground((uint8_t*)deviceSN,
                                                        strlen(deviceSN),CDC0_INTFNUM,0);
                        }

                        else if (wholeString[1] =='H') {
                            printHelp();
                        }


                        else {
                            // Prepare the String
                            strcpy(outString,"\r\nPressed Command String\r\n\r\n");
                        }
                        // Send the String to USB
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;

                    default :
                        strcpy(outString,"\n");                                                         // Send new Line when return pressed
                        USBCDC_sendDataInBackground((uint8_t*)outString,
                                                    strlen(outString),CDC0_INTFNUM,0);
                        break;
                    }

                    // Clear the string in preparation for the next one
                    for (i = 0; i < MAX_STR_LENGTH; i++){
                        wholeString[i] = 0x00;
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

/*
 * Get the data buffer array and according to data set the leds
 * @PARAM null
 */
void setLeds() {
    // Counter initialization
    uint8_t i = 0;
    char commandType = 0;
    // Go all over the buffer and search the command A to star parsing for commands
    // Remove 4 bytes from Buffer size to eliminate the chance to address overflow
    for (i = 0; i < BUFFER_SIZE - 4 ; i++ ) {
        // If command A found, go to the next bytes and issue commands to led
        if (dataBuffer[i] == 'A') {
            // go to the command parsing
            commandType = dataBuffer[i+1];

            switch(commandType) {
            // Toggle command
            case 't'  :
                // Evaluate the led number and toggle it On / off
                switch(dataBuffer[i+2]) {
                // Case of LED R
                case 'r' :
                    GPIO_toggleOutputOnPin(LED_PORT,LED_R);
                    break;
                    // Case of LED G
                case 'g' :
                    GPIO_toggleOutputOnPin(LED_PORT,LED_G);
                    break;
                    // Case of LED B
                case 'b' :
                    GPIO_toggleOutputOnPin(LED_PORT,LED_B);
                    break;
                }
                break; // Break toggle command
                // Turn on
                case  'a' :
                    led = dataBuffer[i+3];
                    Timer_A_stop(TIMER_A0_BASE);
                    Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2,0xEEEE);
                    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_CONTINUOUS_MODE);
                    //statement(s);
                    break; /* optional */
                case 'l' :
                    // Statement
                    break;



            } // Close parsing of command start switch case


        } // Close if loop test for 'A'
    } // Close for loop

    GPIO_toggleOutputOnPin(GPIO_PORT_P1,dataBuffer[0]);

}

/*
 *   int i ;
                    // If received 'B' will identify itself as fit. Temporary later will add information function retrieval
                    if (dataBuffer[0] == 'B'){
                    	while (dataBuffer[0] != 'C')
                    	{
                    		USBCDC_sendDataInBackground("fit",3,CDC0_INTFNUM,1);
                    		USBCDC_receiveDataInBuffer((uint8_t*)dataBuffer,BUFFER_SIZE,CDC0_INTFNUM);
                    	}
                    }
                    else
                    	USBCDC_receiveDataInBuffer((uint8_t*)dataBuffer,BUFFER_SIZE,CDC0_INTFNUM);
 */
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

    // add data to String of SN to test
    //strcat(deviceSN,"56987\t Rev.1.0\n\n\r");
    // Print device SN
    USBCDC_sendDataInBackground((uint8_t*)deviceSN,
                                strlen(deviceSN),CDC0_INTFNUM,0);

    //strcpy(outString,"SN:deviceSN\t\t\t1235456\n\rDATE:\t\t\t03/01/2017\n\rProgram Revision:\t1.0\n\rHW Revision:\t\t1.0\n\rRed: 0xFFFF\tGreen: 0xFFFE\tBlue: 0xFFFF\n\n\r");
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    // Available commands, Broken to many line to simplify the look of the program
    // help command
    strcpy(outString,"\n-H\t\t\t-will print this screen\r\n\r\n");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);
    // device SN
    strcpy(outString,"-V\t\t\t-Print the device sn and revision\r\n\r\n");
    // Send the response over USB
    USBCDC_sendDataInBackground((uint8_t*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

    strcpy(outString,"# HTML Color Code - #FFFFFF turn all the LED's ON (White)\t\t\t\r\n\r\n");
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

