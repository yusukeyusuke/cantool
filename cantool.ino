// CAN tool
//

#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string
int press = 0;

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(15);                               // Set CS to pin 15

#define BUTTONPIN 0

byte senddata[4] = {0xde, 0xad, 0xba, 0xad};
byte senddatalong[8] = {0xde, 0xad, 0xba, 0xad,0xde, 0xad, 0xbe, 0xef};

void setup()
{
  pinMode(BUTTONPIN, INPUT);
  Serial.begin(115200);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%08X  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%03X       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%02X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }

  if(!digitalRead(BUTTONPIN)) {
    if(press == 0) {
      press = 1;
      Serial.print("PUSHPUSH\n");
      byte sndStat = CAN0.sendMsgBuf(0x100,0,4, senddata);
      if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
      } else {
        Serial.println("Error Sending Message...");
      }   
    }
  }else{
      press = 0;
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
