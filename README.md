# JKBMS-python-arduino-example
A python and arduino script to learn JK BMS communication, use with JK documentation

  ## JK BMS Connector: UART-TTL socket (4 Pin, JST MX 1.25mm pitch)
  https://www.amazon.com/Micro-Connector-Socket-1-25mm-Female/dp/B09DYLY95R
  * remove VBAT or 4th pin to be safe, it is a high voltage line

```
┌─── ─────── ────┐
│                │
│ O   O   O   O  │
│GND  RX  TX VBAT│
└────────────────┘
  │   │   │
  │   │   └─── GPIO39 (`JK_RX`)
  │   └─────── GPIO40 (`JK_TX`)
  └─────────── GND
```


  ## Python:
Relies on pyserial
Change "JKBMS_port" to correct port connected to JKBMS
Need TTL UART Converter Cable.
If using RS485 converter with JK BMS, you will need to use USB to RS485 converter cable.

## Arduino:
Used esp32s3 dev board.
Should be compatible with esp32

Both arduino demos use second serial port of board. The JKBMS_header_demo.ino uses RTOS multitasking
