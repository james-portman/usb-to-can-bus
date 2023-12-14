/*
TODO:
auto baud detection in silent mode
maybe make it use elm327 protocol but with additional options and support e.g. 1mbit baud


*/

#include <SPI.h>
#include "src/arduino-mcp2515/mcp2515.h"
// autowp version of the can library
// https://github.com/autowp/arduino-mcp2515


MCP2515 mcp2515(17);

// software CAN ID filter
// will be slower than hardware but easy to implement
unsigned int filterIDs[] = {
//   0x300,
//   0x301,
//   0x302,
//   0x303,
//   0x304,
//   0x305,
//   0x306,
//   0x307,
//   0x308,
//   0x309,
//   0x30a,
//   0x30b,
//   0x30c,
//   0x30d,
//   0x30e,
//   0x30f,
//   0x310
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

    // check (software) filters
    for (unsigned int i=0; i<sizeof(filterIDs) / sizeof(unsigned int); i++) {
      if (canMsg.can_id == filterIDs[i]) {
        #ifdef DEBUG
          Serial.println("Skipping CAN packet due to filters");
        #endif
        return;
      }
    }

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

#define COMMAND_BUFFER_MAX_SIZE 100
char commandBuffer[COMMAND_BUFFER_MAX_SIZE];
unsigned int commandBufferLen = 0;

void checkSerialCommand() {
  // read incoming serial up to new line then parse
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      parseCommand();
      commandBufferLen = 0;
    } else {
      commandBuffer[commandBufferLen] = c;
      commandBufferLen++;
      if (commandBufferLen >= COMMAND_BUFFER_MAX_SIZE) {
        Serial.println("ERROR command buffer overflow, wiping buffer");
        commandBufferLen = 0;
      }
    }
  }
}
void parseCommand() {
  // some format for commands, copy elm327 ish?
  // ATSP~ to set baud?

  // what format for sending messages out? do the same as receive? or allow binary raw somehow?
  // all in ascii hex
  // [id 000-FFF] [len 00-08] [data bytes...]
  // for now make them send all data bytes even if not used

  // e.g. 307 08 03 E9 00 00 00 00 06 72
  // length 30
  if (commandBufferLen != 30) {
    Serial.println("ERROR incorrect command length, format: [CAN ID] [data len] [(always 8) data bytes...] e.g. 300 02 03 E9 00 00 00 00 00 00");
    return;
  }

  char id[4] = {commandBuffer[0], commandBuffer[1], commandBuffer[2], 0};
  char len[3] = {commandBuffer[4], commandBuffer[5], 0};
  char d0[3] = {commandBuffer[7], commandBuffer[8], 0};
  char d1[3] = {commandBuffer[10], commandBuffer[11], 0};
  char d2[3] = {commandBuffer[13], commandBuffer[14], 0};
  char d3[3] = {commandBuffer[16], commandBuffer[17], 0};
  char d4[3] = {commandBuffer[19], commandBuffer[20], 0};
  char d5[3] = {commandBuffer[22], commandBuffer[23], 0};
  char d6[3] = {commandBuffer[25], commandBuffer[26], 0};
  char d7[3] = {commandBuffer[28], commandBuffer[29], 0};

//  Serial.println(id);
//  Serial.println(len);
//  Serial.println(d0);
//  Serial.println(d1);
//  Serial.println(d2);
//  Serial.println(d3);
//  Serial.println(d4);
//  Serial.println(d5);
//  Serial.println(d6);
//  Serial.println(d7);

  int idInt = (int) strtol(id, 0, 16);
  int lenInt = (int) strtol(len, 0, 16);
  int d0Int = (int) strtol(d0, 0, 16);
  int d1Int = (int) strtol(d1, 0, 16);
  int d2Int = (int) strtol(d2, 0, 16);
  int d3Int = (int) strtol(d3, 0, 16);
  int d4Int = (int) strtol(d4, 0, 16);
  int d5Int = (int) strtol(d5, 0, 16);
  int d6Int = (int) strtol(d6, 0, 16);
  int d7Int = (int) strtol(d7, 0, 16);

  sendMessage(idInt, lenInt, d0Int, d1Int, d2Int, d3Int, d4Int, d5Int, d6Int, d7Int);

  Serial.print("OK ");
  #ifdef DEBUG
    if (idInt < 0x10) { Serial.print("0"); }
    if (idInt < 0x100) { Serial.print("0"); }
    Serial.print(idInt, HEX);
    Serial.print(" ");
    if (lenInt < 0x10) { Serial.print("0"); }
    Serial.print(lenInt, HEX);
    Serial.print(" ");
    if (d0Int < 0x10) { Serial.print("0"); }
    Serial.print(d0Int, HEX);
    Serial.print(" ");
    if (d1Int < 0x10) { Serial.print("0"); }
    Serial.print(d1Int, HEX);
    Serial.print(" ");
    if (d2Int < 0x10) { Serial.print("0"); }
    Serial.print(d2Int, HEX);
    Serial.print(" ");
    if (d3Int < 0x10) { Serial.print("0"); }
    Serial.print(d3Int, HEX);
    Serial.print(" ");
    if (d4Int < 0x10) { Serial.print("0"); }
    Serial.print(d4Int, HEX);
    Serial.print(" ");
    if (d5Int < 0x10) { Serial.print("0"); }
    Serial.print(d5Int, HEX);
    Serial.print(" ");
    if (d6Int < 0x10) { Serial.print("0"); }
    Serial.print(d6Int, HEX);
    Serial.print(" ");
    if (d7Int < 0x10) { Serial.print("0"); }
    Serial.print(d7Int, HEX);
  #endif
  Serial.println();
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
