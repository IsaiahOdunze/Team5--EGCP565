#define BLYNK_PRINT Serial
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 6 //for the led strip
#define inputPin 7 // for the PIRSensor
#define lightPin A0 //for the photoresistor

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

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
#include <SoftwareSerial.h>//for serial connection to esp module
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

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


bool pinValue; //used to switch LED on or off
uint8_t r, g, b; ///set LED Colors
bool pirState = LOW;  // we start, assuming no motion detected for PIR sensor
bool pir_val = 0;//assuming no motion initially

uint16_t smartLighting;//for lighting using photoresistor
uint16_t lightVal;//light intensity
bool theaterMode;//theater mode feature for led lights

uint8_t colorChoice;

//variables to calculate the no motion
long int finishnomtion = 0;
long int nomotiontime = 0;
long int startmotion = 0;


BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1)//virtual pin that sets turns the led on or off.
{

  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();

  // Serial.print("V1 Slider value is: ");
  // Serial.println(pinValue);
  if (pinValue == true) {
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
  //
  //  Serial.print("V2 Slider value is: ");
  //  Serial.print("r : ");
  //  Serial.print(r);
  //  Serial.print(", g : ");
  //  Serial.print(g);
  //  Serial.print(", b : ");
  //  Serial.println(b);
  if (pinValue == 1) {
    colorWipe(strip.Color(r, g, b), 50); // turn on
  }
}

  BLYNK_READ(V3) { //
    pir_val = digitalRead(inputPin);//check motion
    lightVal = analogRead(lightPin);//check ligght intensity
    Blynk.virtualWrite(V3, pir_val);
    if (smartLighting == 1) { ///check if smart lighting is on
      if (pinValue == 1) {//if LED is initially on
        if (lightVal > 750) { // turn off because room is already bright
          colorWipe(strip.Color(0, 0, 0), 50); //turn off
          Blynk.virtualWrite(V1, 0);//turn blynk app button off
          Blynk.notify("Smart Lighting : turning LED OFF");
          pinValue = 0;
        }
        else if (lightVal < 300 && pir_val == 0) {
          finishnomtion++;//
          nomotiontime = finishnomtion - startmotion; //calculate time of no motion
          if (nomotiontime == 10) {
            colorWipe(strip.Color(0, 0, 0), 50); //turn off
            Blynk.virtualWrite(V1, 0);//turn blynk app button off
            Blynk.notify("Smart Lighting : turning LED OFF");
            pinValue = 0;
          }
          //delay(1000);
          startmotion = 0;//start the timing at motion detecting
          startmotion++;
          finishnomtion = 0;
        }
        else if (lightVal < 300 && pir_val == 1) {//reset timer
          nomotiontime = 0;
          startmotion = 0;//start the timing at motion detecting
          startmotion++;
          finishnomtion = 0;
        }
      }
      else {// if LED is intitally off
        if (lightVal < 300 && pir_val == 1) {
          colorWipe(strip.Color(r, g, b), 50); // turn on
          Blynk.virtualWrite(V1, 1); //turn led buttion on
          Blynk.notify("Smart Lighting : turning LED ON");
          pinValue = 1;
        }
      }
    }
  }


  BLYNK_WRITE(V4)// turn on smart lighting
  {
    smartLighting = param.asInt();//check smart lighting value
  
  }

  BLYNK_WRITE(V6)// color Setter
  {
    if (pinValue == 1) {//only if led is on
      colorChoice = param.asInt();
      // Serial.println(colorChoice);
      colorChoice = colorChoice - 1;
      //colorPicker = param.asStr();
      //  Serial.print("Color: ");
      //  Serial.print(color);
      //  Serial.print(" Length: ");
      //  Serial.println(color.length());
      r, g, b = colorPicker(colorChoice);
      //    Serial.print(r);
      //    Serial.print(" ");
      //    Serial.print(g);
      //    Serial.print(" ");
      //    Serial.println(b);
      colorWipe(strip.Color(r, g, b), 50);
    }
  }


  void setup()
  {
    // Debug console
    Serial.begin(9600);

    // Set ESP8266 baud rate
    EspSerial.begin(ESP8266_BAUD);
    delay(10);
    // You can also specify server:
    //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 80);
    //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);
    // your hardware gets connected to Blynk Server
    setupLED();
    pinMode(inputPin, INPUT); // declare PIR sensor as input
    pinMode(lightVal, INPUT); // declare photoresistor as input
    Blynk.begin(auth, wifi, ssid, pass);
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
      strip.show();
      delay(wait);
    }
    //strip.show();
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
        }  // your hardware gets connected to Blynk Server
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
