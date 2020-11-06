#include <FlexCAN.h>
#include <Keypad.h>

static CAN_message_t msg;
int txCount,rxCount;

const unsigned int CANSpeed = 500000;

// left, down, right, up
// use j-pac defaults for player1 & player2
uint16_t _arrows[4][4] = {
  {KEY_LEFT, KEY_DOWN, KEY_RIGHT, KEY_UP},
  {'d', 'f', 'g', 'r'}
}; 

FlexCAN CANbus(CANSpeed);

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

  CANbus.begin();
  Keyboard.begin();
  
  Serial.println(F("Teensy Start"));
}


void loop() {
  // put your main code here, to run repeatedly:

  while (CANbus.available()) {
    CANbus.read(msg);
    Serial.print("BUS: "); hexDump(3, msg.buf);
    
    if (msg.buf[2] == 1) {
      //Serial.println("Pressed");
      Keyboard.press(_arrows[msg.buf[0]][msg.buf[1]]);
    }
    else {
      //Serial.println("unPressed");
      Keyboard.release(_arrows[msg.buf[0]][msg.buf[1]]);
    }

    rxCount++;
  }
  
}