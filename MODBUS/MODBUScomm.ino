#include <ModbusMaster.h>

const int MAX485_DE_RE = 4;  
const int RX_PIN = 16;
const int TX_PIN = 17;

const int LED_PIN = 2; 

ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_DE_RE, HIGH); 
}

void postTransmission() {
  digitalWrite(MAX485_DE_RE, LOW);  
}

void setup() {
  Serial.begin(115200); 

  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW); 

  pinMode(LED_PIN, OUTPUT);

  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  node.begin(1, Serial1); 
  
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println(" Modbus Master Started ");
}

void loop() {
  uint8_t result;
  

  result = node.readHoldingRegisters(0x5500, 1);
  
  if (result == node.ku8MBSuccess) {
    uint16_t data = node.getResponseBuffer(0);
    
    Serial.print("VP 5500 Value: ");
    Serial.println(data);
    
    if (data == 1) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  } else {
    Serial.print("Read Failed. Error: ");
    Serial.println(result, HEX);
  }
  
  delay(500); 
}
