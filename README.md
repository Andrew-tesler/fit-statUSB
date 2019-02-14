## Welcome to GitHub fit-statUSB page

[![GitHub version](https://img.shields.io/github/release-pre/Andrew-tesler/fit-statUSB.svg?style=flat)](https://github.com/Andrew-tesler/fit-statUSB/releases/latest)
[![GitHub download](https://img.shields.io/github/downloads-pre/Andrew-tesler/fit-statUSB/latest/total.svg?style=flat)](https://github.com/Andrew-tesler/fit-statUSB/releases/latest)
[![GitHub issues](https://img.shields.io/github/issues-raw/Andrew-tesler/fit-statUSB.svg?style=flat)](https://github.com/Andrew-tesler/fit-statUSB/issues)
[![GitHub last commit](https://img.shields.io/github/last-commit/Andrew-tesler/fit-statUSB.svg?style=flat)](https://github.com/Andrew-tesler/fit-statUSB/commits/master)

* fit-statUSB is a programmable fingernail-size color LED that plugs into a USB port.
* fit-statUSB provides status indication by changing the LED color, brightness and color cycle.
* fit-statUSB identifies to the PC as a serial device and changes the LED color by text commands received over the serial port.
* See [https://fit-iot.com/web/product/fit-statusb/ fit-statUSB product page]


# Commands
## General 
Commands to fit-statUSB are sent over serial port. No need to configure the baudrate, fit-statUSB adjusts automatically.
Each command ends with a LF and executed immediately.

### Set color

 #RRGGBB
* RR | GG | BB - HEX value of red, green, blue respectively
fit-statUSB will set the intensity of each of the colors.



Examples:
To set dim green 
```
 #001200
```

To set bright yellow combine red and green
```
 #FFFF00
```

### Set color sequence 
Set a sequence of alternating colors. The sequence will cycle endlessly until a new color command is entered.
```
B#RRGGBB-tttt#RRGGBB....
```

* RR | GG | BB - HEX value of red, green, blue respectively
* tttt - time until next color is displayed

### Examples:

Pulsing red color
```
B#FF0000-0200#000000-0600
```
Alternating red-blue
```
B#FF0000-1000#0000FF-1000
```
Cycle of red 0.1 sec - yellow 0.1 sec -white 1 sec - off 0.3 sec
```
B#FF0000-0100#FFFF00-0100#FFFFFF-1000#000000-0300
```

### Set color transition period 
Set how quickly current color fades to the next color.
```
 Fnnnn
```

* nnnn - transition time in mS. Permitted values: 0001 (immediate) to 9999 (slowest transition, 10 seconds)
Transition period is persistent until a new F command is entered. <br>
For B command the F value will determine transition period between colors.

### other (less common) commands

* P - Enter Device firmware update mode
* ? - Return device UUID #
* G - Return current color, (rr,gg,bb)

## Using fit-statUSB in Windows 
### Drivers ===
* [https://fit-iot.com/files/download/accessories/fit-statusb/sw/Win7/msp430_ti_signed.inf Fit-statUSB Windows 7 driver]
* [https://fit-iot.com/files/download/accessories/fit-statusb/sw/Win8.1/msp430_cdc.zip Fit-statUSB Windows 8.1 driver]
* Additional drivers are not required for Windows 10 operating systems

### Using fit-statUSB from terminal ===
* Open a terminal utility with serial port support (e.g. [https://ttssh2.osdn.jp Teraterm])
* Establish new connection to the serial port assigned to fit-statUSB (in TeraTerm: File --> New connection --> Serial (select COMn: USB Serial Device)
* Type a command, e.g. #FF0000 (fit-statUSB should turn RED)
Note there is no command editing (no backspace / delete). If you mistype please start a new command.
### Using fit-statUSB from command line ===

```
 echo [command] > \\.\[com port]
```

* [command] - the command sent to fit-statUSB over serial
* [com port] - the COM port assigned to fit-statUSB

### Example:
Assuming fit-statUSB is on COM10 the following command will set fit-statUSB color to red:
```
 echo #FF0000 > \\.\COM10
```
### Detect COM port of fit-statUSB 
#### From Device Manager 
Under device manager | Ports (COM & LPT)<br>

look for

USB Serial Device (COMn)

* n - COM port number
#### From command line 
To identify which COM port is assigned to fit-statUSB use '''mode''' command
 ```
 mode
```
You will receive a list of COM ports. fit-statUSB listing on COM10 looks like that:

 Status for device COM10:
 ------------------------
    Baud:            9600
    Parity:          None
    Data Bits:       8
    Stop Bits:       1
    Timeout:         ON
    XON/XOFF:        OFF
    CTS handshaking: OFF
    DSR handshaking: OFF
    DSR sensitivity: OFF
    DTR circuit:     OFF
    RTS circuit:     OFF

### PowerShell 
```
 powershell "$fitstatUSB= new-Object System.IO.Ports.SerialPort [com port]; $fitstatUSB.open(); $fitstatUSB.WriteLine('[command]'); $fitstatUSB.Close()"
```
* [command] - the command sent to fit-statUSB over serial
* [com port] - the COM port assigned to fit-statUSB

### Example:
Assuming fit-statUSB is on COM10 the following command will set fit-statUSB color to red:
 powershell "$fitstatUSB= new-Object System.IO.Ports.SerialPort COM10; $fitstatUSB.open(); $fitstatUSB.WriteLine('#FF0000'); $fitstatUSB.Close()"

## Using fit-statUSB in Linux 
### Using fit-statUSB from terminal 
#### Using Bash Command line 
* First you need to configure the Serial port 
 ```
 sudo stty -F /dev/ttyACM0 9600 raw -echo -echoe -echok -echoctl -echoke
```
* Send the command directly to the TYY port
```
echo "B#FF0000-1000#00FF00-500" > /dev/ttyACM0
```
- Please note that some Linux distributions will require sudo privileges to echo directly to serial

==== Sample Python code ====
* Python dependencies: pyserial
 ```
 import serial<br>
 ser = serial.Serial("/dev/ttyACM0")
 ser.write(b"B#FF0000-500#00FF00-500#0000FF-500\n")
 ser.close()
 ```
- Note that new line must be addded at the end of the command String

### Detect fit-statUSB Serial Port 
To Detect the system assigned TTY device:
 ```
 sudo dmesg 
 ```
Look for fit_StatUSB String and its assigned TTY device

