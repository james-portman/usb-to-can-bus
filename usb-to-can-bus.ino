#include <SPI.h>
#include "src/arduino-mcp2515/mcp2515.h"
// autowp version of the can library
// https://github.com/autowp/arduino-mcp2515


MCP2515 mcp2515(17);

int filterIDs[] = {
  // 0x300
};


void setup() {
  Serial.begin(115200); // dont think baud matters, will be full rate because CDC
 
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS);
  mcp2515.setNormalMode();

}

void loop() {
  readIncoming();
  checkSerialCommand();
}

void readIncoming() {
  struct can_frame canMsg;
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

    // TODO: check software IDs filter to ignore

    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" ");
    if (canMsg.can_dlc < 0x10) {
      Serial.print("0");
    }
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
      if (canMsg.data[i] < 0x10) {
        Serial.print("0");
      }
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }

    Serial.println();      
  }
}

void checkSerialCommand() {
  // what format for sending messages out? do the same as receive? or allow binary raw somehow?
  // all in ascii hex
  // [id 000-FFF] [len 00-08] [data bytes...]
  // read incoming serial up to new line then parse
  if (Serial.available() > 0) {
    Serial.read();
    if == '\n' {
      parse command
    }
  }
}

void sendMessage(int id, int len, int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7) {
  struct can_frame canMsg1;
  canMsg1.can_id  = id;
  canMsg1.can_dlc = len;
  canMsg1.data[0] = d0;
  canMsg1.data[1] = d1;
  canMsg1.data[2] = d2;
  canMsg1.data[3] = d3;
  canMsg1.data[4] = d4;
  canMsg1.data[5] = d5;
  canMsg1.data[6] = d6;
  canMsg1.data[7] = d7;
  mcp2515.sendMessage(&canMsg1);
}
