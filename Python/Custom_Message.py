"""
Description:
    This script uses pyserial library and a USB to Serial device to read specified
    JK BMS data. It does the necessary calculations for performing the checksum and frame length
    in order to send a valid BMS data request.

Communication Rules:
    Refer to the communication data format for details. The interval between 
    packets shall be at least 100ms, 
    and the longest reply packet shall not exceed 5S. Broadcast regularly. If it 
    is sleeping, send activation 
    information at the control end, activate BMS, and then communicate

FOR ARDUINO USE:
    It prints out the raw bytes so you can copy and paste the formmatted message into an Arduino script.
    For example:

    const uint8_t jkbms_current_message[] = {
        0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x03, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00,
        0x68, 0x00, 0x00, 0x01, 0xAA
    };

    Which can be send over serial to BMS. I recommend to use alternative uart controller or reassign pins of main serial.
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

Command_Code = 0x03 #0x03 is Read BMS ID Data, 0x06 is Read All Data
Transmission_Type = 0 #See JKBMS RS485 Communication Protocol Documentation
Frame_Information_Unit = [0x03] #BMS ID Data Code

pre_frame_length_message = bytes([
    0x00, 0x00, 0x00, 0x00,
    Command_Code, 
    0x03, 
    Transmission_Type, 
    *Frame_Information_Unit, 
    0x00, 0x00, 0x00, 0x00,
    0x68
])

### Calculate Frame_Length
total = 2+len(pre_frame_length_message)+4 #add 2 because frame_length itself, add 4 because of checksum length
# Mask to 2 bytes and format
two_byte_sum = total & 0xFFFF
high_byte = (two_byte_sum >> 8) & 0xFF
low_byte = two_byte_sum & 0xFF
Frame_Length = [high_byte, low_byte]

pre_checksum_message = bytes([
    0x4E, 0x57, 
    *Frame_Length, 
    *pre_frame_length_message
])

### Calculate Checksum, from instructions high 2-byte of CRC16 is not used, first two high bytes = zero always
total = sum(pre_checksum_message)
# Mask to 2 bytes and format
two_byte_sum = total & 0xFFFF
high_byte = (two_byte_sum >> 8) & 0xFF
low_byte = two_byte_sum & 0xFF
Checksum = [0x00, 0x00, high_byte, low_byte]

data = bytes([
    *pre_checksum_message,
    *Checksum
])

print("Message Sent to BMS:")
# print(data)
print("Hex:", ' '.join(f'{b:02X}' for b in data))

JKBMS.write(data)

time.sleep(0.1)

# Read all available response bytes
response = JKBMS.read_all()

# Print the raw response in hexadecimal
print("BMS Response:")
print("Hex:", ' '.join(f'{b:02X}' for b in response))