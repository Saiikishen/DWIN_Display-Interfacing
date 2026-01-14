#include <HardwareSerial.h>

const byte rxPin = 16; 
const byte txPin = 17; 
HardwareSerial dwin(1);

const int LED_PIN = 2;
const uint8_t FRAME_SIZE = 9;

volatile bool frameReady = false;
volatile unsigned char rxBuffer[FRAME_SIZE];
volatile uint8_t rxIndex = 0;

void IRAM_ATTR onDWINReceive() {
  while (dwin.available() && rxIndex < FRAME_SIZE) {
    rxBuffer[rxIndex++] = dwin.read();
    if (rxIndex >= FRAME_SIZE) {
      frameReady = true;
      rxIndex = 0; 
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
  dwin.setClockSource(UART_CLK_SRC_APB);
#endif


  dwin.setRxBufferSize(256);
  
  dwin.begin(115200, SERIAL_8N1, rxPin, txPin);
  

  dwin.setRxTimeout(3); 
  
  dwin.onReceive(onDWINReceive, true);

  Serial.println("DWIN Interrupt Control ");
}

void loop() {

  if (frameReady) {

    unsigned char localBuffer[FRAME_SIZE];
    memcpy(localBuffer, (const void*)rxBuffer, FRAME_SIZE);
    frameReady = false;
    interrupts();
    

    if (localBuffer[0] == 0x5A && 
        localBuffer[4] == 0x55 && 
        localBuffer[5] == 0x00) {
      
      if (localBuffer[8] == 1) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("ON");
      } 
      else if (localBuffer[8] == 0) {
        digitalWrite(LED_PIN, LOW);
        Serial.println("OFF");
      }
    }
  }
}
