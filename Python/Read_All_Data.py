"""
Description:
    This script uses pyserial library and a USB to Serial device to read
    JK BMS data. Uses preformatted Read All Data command to request data from BMS.
"""

import serial
import time

# ================= USER CONFIG =================
# Change this to the correct serial port for your system
# Windows example: 'COM3'
# Linux example: '/dev/ttyUSB0'
# macOS example: '/dev/tty.usbserial-XXXXX'
JKBMS_port = 'COM3'
# ===============================================

JKBMS = serial.Serial(port=JKBMS_port,baudrate=115200,bytesize=8,stopbits=1,parity=serial.PARITY_NONE)

activation_request = bytes([
    0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x68, 0x00, 0x00, 0x01, 0x24
])

read_all_data_request = bytes([
    0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x68, 0x00, 0x00, 0x01, 0x29
])


# Activation Request, still mysterious, helps get solve some startup issues
JKBMS.write(activation_request)
time.sleep(0.1)

JKBMS.write(read_all_data_request)

time.sleep(0.1)

# Read all available response bytes
response = JKBMS.read_all()

# Print the raw response in hexadecimal
print("Hex:", ' '.join(f'{b:02X}' for b in response))

## Decoding Example: Battery Strings and Each Voltage
# Data starts on bit 11
# DATA : 0x79 is Single Battery Voltage (voltage of each battery in series)
print(hex(response[11]))

data_length = int(response[12])   #Length in bytes of how many single battery voltage data will be
battery_series = int(data_length/3) #We can find how many battery strings there are by dividing by 3. Each string will be 3 bytes
print(f"{hex(response[12])} -> {battery_series} Battery Voltages")

voltages_data = response[13:13+data_length]

for i in range (0, data_length, 3):
    series_number = int(voltages_data[i])

    # Bitwise operations
    high_byte = voltages_data[i+1]
    low_byte = voltages_data[i+2]
    millivoltage = (high_byte << 8) | low_byte

    print(f"{series_number} {millivoltage/1000}")