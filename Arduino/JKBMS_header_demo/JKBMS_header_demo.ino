// This demo uses jkbms.h, using RTOS features, creates a task that will request data from BMS every second

#include <jkbms.h>

#define JK_RX 39
#define JK_TX 40

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, JK_RX, JK_TX);

  xTaskCreate(pollBMS, "pollBMS", 2048, (void*)&Serial1, 1, NULL);

}

  float total_battery;
  float current;
  uint8_t state_of_charge;

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available() > 0) {
    JKBMSencode(Serial1.read(), Serial1);
  }

  // // Retrieving data is done by jkbms_log.{} item, see JKBMS.h for types
  // bms_total_voltage = jkbms_log.total_battery; //float 
  // bms_current = jkbms_log.current; //float
  // bms_soc = jkbms_log.state_of_charge; //byte, uint8_t
}