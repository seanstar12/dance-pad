#include <FlexCAN.h>
#include <Keypad.h>

char str[4];
static CAN_message_t msg, rxMsg;
int txCount,rxCount;
uint16_t _arrows[] = {KEY_LEFT, KEY_DOWN, KEY_RIGHT, KEY_UP}; // left, down, right, up

FlexCAN CANbus(500000);

static void hexDump(uint8_t dumpLen, uint8_t *bytePtr) {
  uint8_t hex[17] = "0123456789abcdef";
  uint8_t working;
  
  while (dumpLen--) {
    working = *bytePtr++;
    Serial.write(hex[working >> 4]);
    Serial.write(hex[working & 15]);
  }
  
  Serial.write('\r');
  Serial.write('\n');
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Can0.begin();
  Keyboard.begin();
  
  Serial.println(F("Teensy Start"));
}


void loop() {
  // put your main code here, to run repeatedly:

  while (Can0.available()) {
    Can0.read(msg);
    Serial.print("BUS: "); hexDump(3, msg.buf);
    
    if (msg.buf[2] == 1) {
      //Serial.println("Pressed");
      Keyboard.press(_arrows[msg.buf[1]]);
    }
    else {
      //Serial.println("unPressed");
      Keyboard.release(_arrows[msg.buf[1]]);
    }

    rxCount++;
  }
  
}