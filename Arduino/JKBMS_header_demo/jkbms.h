struct jkbms_log {
  float total_battery;
  float current;
  uint8_t state_of_charge;
} jkbms_log;

uint8_t highByte;
uint8_t lowByte;
uint16_t value;
uint8_t value2;

// Failed to resolve startup issue
// const uint8_t activation_request[] = {
//   0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
//   0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//   0x68, 0x00, 0x00, 0x01, 0x24
// };

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


// NEED TO FIX, ADD WAITING FOR REPLY TO POLL BMS TO SLOW SAMPLING RATE
// IF BATTERY TURNS ON, mid message, it gets confused.

// OR INITIALIZE USING COMMAND WORD on ARDUIN CODE SIDE:
// When the BMS hibernates, the 
// control side must activate the 
// command first to communicate with 
// the BMS. Received 

void pollBMS(void *pvParameters) {
  Stream &port = *(Stream*)pvParameters; //no idea how this line works
  // vTaskDelay(1000 / portTICK_PERIOD_MS);
  // port.write(activation_request, sizeof(activation_request));
  // vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (true) {
    port.write(jkbms_total_battery_voltage_message, sizeof(jkbms_total_battery_voltage_message));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    port.write(jkbms_current_message, sizeof(jkbms_current_message));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    port.write(jkbms_soc_message, sizeof(jkbms_soc_message));
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void JKBMSencode(char c, Stream &port) {
  Serial.println(int(c));
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
      jkbms_log.current = float(value);
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