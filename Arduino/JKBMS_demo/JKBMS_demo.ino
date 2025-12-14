// This example reads from the BMS on Serial1
// And prints it on Serial, back to Serial Monitor of arduino ide

// Define your the RX and TX pins for JK BMS
#define RXD1 39
#define TXD1 40

struct jkbms_log {
  float total_battery;
  float current;
  uint8_t state_of_charge;
} jkbms_log;

uint8_t highByte;
uint8_t lowByte;
uint16_t value;
uint8_t value2;

unsigned long lastTime = 0;

const uint8_t jkbms_read_all_data_message[] = {
  0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
  0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x68, 0x00, 0x00, 0x01, 0x29
};

const uint8_t jkbms_total_battery_voltage_message[] = {
  0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x03, 0x00, 0x83, 0x00, 0x00, 0x00, 0x00,
  0x68, 0x00, 0x00, 0x01, 0xA9
};

const uint8_t jkbms_current_message[] = {
  0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x03, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00,
  0x68, 0x00, 0x00, 0x01, 0xAA
};

const uint8_t jkbms_soc_message[] = {
  0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x03, 0x00, 0x85, 0x00, 0x00, 0x00, 0x00,
  0x68, 0x00, 0x00, 0x01, 0xAB
};


void setup() {
  // Initialize default Serial for debugging
  pinMode(RXD1, INPUT);
  pinMode(TXD1, OUTPUT);
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Main Serial started on USB."); // output to Serial Monitor of arduino ide
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1); // serial input/output to JK BMS
}

void loop() {
    while (Serial1.available() > 0) {
    JKBMSencode(Serial1.read(), Serial1);
  }

  if (millis() - lastTime >= 1000) {  // 1000 ms = 1 second
    lastTime = millis();
    Serial.println("OUTPUTTING!");
    Serial1.write(jkbms_total_battery_voltage_message, sizeof(jkbms_total_battery_voltage_message));
    delay(100);
    Serial1.write(jkbms_current_message, sizeof(jkbms_current_message));
    delay(100);
    Serial1.write(jkbms_soc_message, sizeof(jkbms_soc_message));

    delay(100);
    Serial.print("Voltage: ");
    Serial.print(jkbms_log.total_battery);
    Serial.print("Current: ");
    Serial.print(jkbms_log.current);
    Serial.print("SOC%: ");
    Serial.print(jkbms_log.state_of_charge);
  }
}

void JKBMSencode(char c, Stream &port) {
  uint8_t buffer[2];
  switch(c) {
    case 0x83:
      port.readBytes(buffer, 2);
      highByte = buffer[0];
      lowByte = buffer[1];
      value = (highByte << 8) | lowByte;
      jkbms_log.total_battery = float(value)*0.01;
      break;
    case 0x84:
      port.readBytes(buffer, 2);
      highByte = buffer[0];
      lowByte = buffer[1];
      value = (highByte << 8) | lowByte;
      jkbms_log.current = (10000.0-float(value))*0.01;
      break;
    case 0x85:
      port.readBytes(buffer, 1);
      value2 = buffer[0];
      jkbms_log.state_of_charge = value2;
      break;
    default:
      break;
  }
}