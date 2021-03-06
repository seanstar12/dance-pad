//#include <Streaming.h>
#include <avdweb_AnalogReadFast.h>
#include <Adafruit_NeoPixel.h>
#include <CAN.h>

template <typename T, size_t N> constexpr size_t array_size(const T (&)[N]) { return N; }

uint8_t adcPins[] = {A0, A1, A2, A3};
uint8_t idntPins[] = {9, 8, 7};
float adcValues[] = {0.0, 0.0, 0.0, 0.0};
float adcBaseValues[] = {0.0, 0.0, 0.0, 0.0};
String arrows[] = {"Left", "Down", "Right", "Up"};
String _arrows[] = {"L", "D", "R", "U"};
int prevActive = 0;

char data_pad = 0;     // B0 - player_1; B1 - player_2
char data_arrow = 0;   // B0 - left; B1 - down; B10 - right; B11 - up;
char data_pressed = 0; // B0 - off; B1 - on;

#define LED_PIN 2
#define LED_COUNT 5
#define AVG 20

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

float readAnalogPin(uint8_t pin) {
  float adc = 0;
  int count = 0;

  while(count < AVG) {
    adc += analogReadFast(pin);
    count++;
  }

  return (adc / AVG);
}

void readAnalogPins(uint8_t pin[], int sizeOfArray) {
 
  for (int k = 0; k < sizeOfArray; k++) {
    float adc = 0;
    int count = 0;
    
    adcValues[k] = readAnalogPin(pin[k]);
  }

  // test statements
  if (false) {
    Serial.print(adcValues[0]);
    Serial.print("\t");
    Serial.print(adcValues[1]);
    Serial.print("\t");
    Serial.print(adcValues[1]);
    Serial.print("\n");
  }
}


void setup(void) { 
  Serial.begin(115200);
  // init neopixels 
  strip.begin();
  strip.show();

  // set pullups for adc sensors
  for (int i = 0; i < 4; i++) {
    pinMode(adcPins[i], INPUT_PULLUP);
  }

  // set pullups for identity pins
  for (int m = 0; m < 3; m++) {
    pinMode(idntPins[m], INPUT_PULLUP);
  }


  // Read identity pins for config
  // left, down, up, right
  data_pad = digitalRead(9) == HIGH ? 1 : 0;
  data_pad += digitalRead(7) == HIGH ? 2 : 0;
  data_pad += digitalRead(8) == HIGH ? 4 : 0;
  
  // take AVG readings
  for (int k = 0; k < AVG; k++) {
    readAnalogPins(adcPins, 4);

    // read each sensor
    for (int j = 0; j < 4; j++) {
      adcBaseValues[j] += adcValues[j];
    }
  }

  // average each sensor
  for (int l = 0; l < 4; l++) {
    adcBaseValues[l] = (adcBaseValues[l] / AVG);
  }

  if (false){
    Serial.print("Base Values: ");
    Serial.print(adcBaseValues[0]);
    Serial.print("\t");
    Serial.println(adcBaseValues[1]);
    delay(200);
  }

  if (true) {
  Serial.print("Player: ");
  Serial.print(bitRead(data_pad, 0) + 1, HEX);
  Serial.print("\t Arrow: ");
  Serial.println(_arrows[data_pad >> 1]);
  //Serial.print("\t");
  //Serial.print("Arrow");

  }
  
  if (!CAN.begin(500000)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop(void) {
  int active = 0;
  
  readAnalogPins(adcPins, 4);

  // if 25% change in value, trigger 'sensor pressed'
  for (int i = 0; i < 4; i++) {
    if ((adcBaseValues[i] - adcBaseValues[i]*0.25) > adcValues[i]) {
      active++;
    }
  }

  //set green pixel if any sensor is triggered (debug)
  if (active > 0) {
    strip.setPixelColor(4, 60, 0, 0);
  }
  else {
    strip.setPixelColor(4, 0, 0, 0);
  }

  // send change over CANBus 
  if (active != prevActive) {
    CAN.beginPacket(0x12);
    CAN.write(bitRead(data_pad, 0));
    CAN.write(data_pad >> 1);
    CAN.write((active > 0) ? 0x01 : 0x00);
    CAN.endPacket();
  }
  
  prevActive = active; 

  // set 'pressure' from sensor
  // blue if pressed, red if hard pressed.
  // @TODO remove in final, testing only
  for (int i = 0; i < 4; i++) {
    int val = 1024 - adcValues[i];
    int r = 0;
    int g = 0;
    int b = 0;

    if (val < 510) {
      b = val / 2;
    }
    else if ( val >= 510 ) {
      r = map(val - 510, 0, 512, 0, 250);
      b = (r <= 200) ? 200 - r : 0;
    }
    strip.setPixelColor(i, g, r, b);
  }

  // render neopixels 
  strip.show();

  
  if (false) {
    Serial.print(adcValues[0]);
    Serial.print("\t");
    Serial.print(adcValues[1]);
    Serial.print("\t");
    Serial.print(active);
    Serial.print("\n");
  }
}
