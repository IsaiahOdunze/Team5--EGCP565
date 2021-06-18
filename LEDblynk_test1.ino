#define BLYNK_PRINT Serial
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>//for serial connection to esp module
#endif
#define PIN 6

//for the app
//#define BLYNK_TEMPLATE_ID "TMPLmDdQxDDT"
//#define BLYNK_DEVICE_NAME "Smart Lamp"


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "lsbUWAa5sNRDPHT9ckh2wF6Dqs6c9QpQ";


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MySpectrumWiFie8-2G";
char pass[] = "mellowflower024";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
int pinValue; //used to switch LED on or off
int r, g, b; ///set LED Colors
int pirState = LOW;  // we start, assuming no motion detected for PIR sensor
int pir_val = 0;
int inputPin = 7;



BLYNK_WRITE(V1)//virtual pin that sets turns the led on or off.
{
  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();

  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
  if (pinValue == 1) {
      colorWipe(strip.Color(r, g, b), 50);
    }
    else {
    colorWipe(strip.Color(0, 0, 0), 50); // off
  }
}
BLYNK_WRITE(V2)// set the color of our led
{
  r = param[0].asInt(); // assigning r value
  g = param[1].asInt(); // assigning g value
  b = param[2].asInt(); // assigning b value
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();

  Serial.print("V2 Slider value is: ");
  Serial.print("r : ");
  Serial.print(r);
  Serial.print(", g : ");
  Serial.print(g);
  Serial.print(", b : ");
  Serial.println(b);
  if (pinValue == 1) {
    colorWipe(strip.Color(r, g, b), 50); // red
  }
}


void setup()
{
  // Debug console
  Serial.begin(115200);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  setupLED();

  Blynk.begin(auth, wifi, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);
  pinMode(inputPin, INPUT);     // declare sensor as input
  // Clear the terminal content
  //    terminal.clear();
  //
  //    terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  //    terminal.println(F("-------------"));
  //    terminal.flush();
  Blynk.syncAll();
}

void loop()
{
  Blynk.run();
}

void setupLED() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

//Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    //strip.show();
    delay(wait);
  }
  strip.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
