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
 * ======== main.c ========
 * Local Echo Demo:
 *
 * This example simply echoes back characters it receives from the host.  
 * Unless the terminal application has a built-in echo feature turned on, 
 * typing characters into it only causes them to be sent; not displayed locally.
 * This application causes typing in Hyperterminal to feel like typing into any 
 * other PC application ? characters get displayed.  
 *
 * ----------------------------------------------------------------------------+
 * Please refer to the Examples Guide for more details.
 * ---------------------------------------------------------------------------*/
#include <string.h>

#include "driverlib.h"


#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"                 // USB-specific functions
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"


/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"
#include "myTimers.h"

#include "defines.c"

// Global flags set by events
volatile uint8_t bCDCDataReceived_event = FALSE;  // Flag set by event handler to 
volatile uint16_t led;
// indicate data has been
// received into USB buffer
void setLeds();

#define BUFFER_SIZE 256
char dataBuffer[BUFFER_SIZE] = "";
char nl[2] = "\n";


// Function declarations
uint8_t retInString (char* string);
void printHelp(void);

#define MAX_STR_LENGTH 128
char wholeString[MAX_STR_LENGTH] = ""; // Entire input str from last 'return'


// Holds the new addition to the string
char pieceOfString[MAX_STR_LENGTH] = "";

// Holds the outgoing string
char outString[MAX_STR_LENGTH] = "";

//*****************************************************************************
// Global device values
//*****************************************************************************
// Device Serial Number 10 Characters
char deviceSN[25];

#pragma DATA_SECTION ( count, ".infoB" )
uint16_t count;

uint16_t c = 0;



/*----------------------------------------------------------------------------+
 | Main Routine                                                                |
 +----------------------------------------------------------------------------*/
void main (void)
{
	WDT_A_hold(WDT_A_BASE); // Stop watchdog timer

	// Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
	PMM_setVCore(PMM_CORE_LEVEL_3);


	USBHAL_initPorts();           // Config GPIOS for low-power (output low)
	USBHAL_initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
	USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect
	// Initialize timers



	__bis_SR_register( GIE );                                                   // Enable interrupts globally

	int tempR,tempR2,tempG,tempG2,tempB,tempB2 = 0;

	 c = tempG;

	 initTimers(0,25,0);

	__enable_interrupt();  // Enable interrupts globally

	// Gather information from the card
	//strcpy(deviceSN,"Serial No:\t\t\t1234567890\n\r");
	strcpy(deviceSN,"Device SN: 56987\t Rev.1.0\r\n\r\n");


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

					// Compare to string #1, and respond
					if (!(strcmp(wholeString, "b on"))){

					    //GPIO_setOutputLowOnPin(LED_PORT,LED_R);
					    GPIO_setOutputLowOnPin(LED_PORT,LED_B);
//						initTimers(0,0,128);

						// Prepare the outgoing string
						strcpy(outString,"\r\nBlue is ON\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);

						// Compare to string #2, and respond
					} else if (!(strcmp(wholeString, "b off"))){

//						initTimers(0,0,0);

					    //GPIO_setOutputHighOnPin(LED_PORT,LED_R);
					    GPIO_setOutputHighOnPin(LED_PORT,LED_B);

						// Prepare the outgoing string
						strcpy(outString,"\r\nBlue is OFF\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);


					} else if (!(strcmp(wholeString, "r on"))){

					    GPIO_setAsOutputPin(LED_PORT,LED_R);
					    GPIO_setOutputLowOnPin(LED_PORT,LED_R);
						//initTimers(128,0,0);
						// Prepare the outgoing string
						strcpy(outString,"\r\nRed is On\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);
					} else if (!(strcmp(wholeString, "r off"))){

					    GPIO_setAsInputPin(LED_PORT,LED_R);
					    GPIO_setOutputHighOnPin(LED_PORT,LED_R);
//						initTimers(0,0,0);

						// Prepare the outgoing string
						strcpy(outString,"\r\nRed is Off\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);

					} else if (!(strcmp(wholeString, "g on"))){
					    GPIO_setOutputLowOnPin(LED_PORT,LED_G);
//						initTimers(0,128,0);

						// Prepare the outgoing string
						strcpy(outString,"\r\nGreen is On\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);

					} else if (!(strcmp(wholeString, "g off"))){
					    GPIO_setOutputHighOnPin(LED_PORT,LED_G);
//						initTimers(0,0,0);

						// Prepare the outgoing string
						strcpy(outString,"\r\nGreen is Off\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);

						// Compare to string #3, and respond
					} else if (!(strcmp(wholeString, "-v"))){

						//strcat(deviceSN,"56987\t Rev.1.0\n\n\r");
						// Print device SN
						USBCDC_sendDataInBackground((uint8_t*)deviceSN,
								strlen(deviceSN),CDC0_INTFNUM,0);

						// Compare string and respond
					} else if (!(strcmp(wholeString, "-h"))){

						// Print help screen
						printHelp();

					} else if (wholeString[0] == '#'){
						// Test Function for Fade in amd out the leds

						// Turn off timer while changing toggle period
						Timer_A_stop(TIMER_A0_BASE);



						tempR =  wholeString[1] - '0';
						tempR2 =  wholeString[2] - '0';
						tempR = (tempR2 * 10) + tempR;

						tempG =  wholeString[3] - '0';
						tempG2 =  wholeString[4] - '0';
						tempG = (tempG2 * 10) + tempG;

						tempB =  wholeString[5] - '0';
						tempB2 =  wholeString[6] - '0';
						tempB = (tempB2 * 10) + tempB;


						initTimers(tempR,tempG,tempB);



//						dof
//						FlashCtl eraseSegment(FlashCtl BASE,
//						(unsigned char *)INFOD START
//						);
//						status = FlashCtl performEraseCheck(FlashCtl BASE,
//						(unsigned char *)INFOD START,
//						128
//						);
//						gwhile(status == STATUS FAIL);
//						//Flash write
//						FlashCtl write32(FlashCtl BASE,
//						calibration data,
//						(unsigned long *)(INFOD START),1);


						//GPIO_toggleOutputOnPin(LED_PORT,LED_G);
						// Prepare the outgoing string
						strcpy(outString,
								"\r\nColor Changed\n\n\r");


						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);




						// Compare to string #4, and respond

					} else if (!(strcmp(wholeString, "prog"))){
						//	Enter Bsl mode (Will reset the USB)

						USB_disconnect();							// Disconnect USB device
						__disable_interrupt();						// Disable global interrupt
						((void (*)())0x1000)();						// Set the bsl address
						USB_connect();								// Connect the USB back on



						// Compare to string #4, and respond


					} else {

						// Prepare the outgoing string
						strcpy(outString,"\r\nNo such command!\r\n\r\n");

						// Send the response over USB
						USBCDC_sendDataInBackground((uint8_t*)outString,
								strlen(outString),CDC0_INTFNUM,0);
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
			// TO DO: User can place code here to handle error
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
	strcpy(outString,"\n-h\t\t\t-will print this screen\r\n\r\n");
		// Send the response over USB
		USBCDC_sendDataInBackground((uint8_t*)outString,
				strlen(outString),CDC0_INTFNUM,0);
		// device SN
		strcpy(outString,"-v\t\t\t-Print the device sn and revision\r\n\r\n");
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

