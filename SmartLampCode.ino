//#define BLYNK_PRINT Serial
#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    60
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
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

//For Light
bool pinValue; //used to switch LED on or off
uint8_t r, g, b; ///set LED Colors
int brightness;

//for smart lighting
uint16_t smartLighting;//for lighting using photoresistor
uint16_t lightVal;//light intensity
bool pirState = LOW;  // we start, assuming no motion detected for PIR sensor
bool pir_val = 0;//assuming no motion initially

//For custom color picker
uint8_t colorChoice;

//For light settings
uint8_t sendSetting;
uint8_t lightSetting;
uint8_t brightSetting;
uint8_t lightingIndex;
bool turnOnLighting;
uint8_t rSetting, gSetting, bSetting;
//char* optionName;
String l_name;
char* options[10] = {"List-Item1", "List-Item2", "List-Item3", "List-Item4", "List-Item5", "List-Item6", "List-Item7", "List-Item8", "List-Item9", "List-Item10"};
uint8_t brightArr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t redArr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t greenArr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t blueArr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//variables to calculate the no motion
//int finishnomtion = 0;
//int nomotiontime = 0;
//int startmotion = 0;
//

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V0) {
  if (pinValue == 1) {
    brightness = param.asInt();
    FastLED.setBrightness(brightness);
    colorLoop(r, g, b);
  }
}

BLYNK_WRITE(V1)//virtual pin that sets turns the led on or off.
{

  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {
    colorLoop(r, g, b);
  }
  else if (pinValue == 0 && turnOnLighting == 0) {
    colorLoop(0, 0, 0); // off
  }
}
BLYNK_WRITE(V2)// set the color of our led
{
  r = param[0].asInt(); // assigning r value
  g = param[1].asInt(); // assigning g value
  b = param[2].asInt(); // assigning b value
  if (pinValue == 1) {
    colorLoop(r, g, b); //change color
  }
}

BLYNK_READ(V3) { //
  if (smartLighting == 1) { ///check if smart lighting is on
    pir_val = digitalRead(inputPin);//check motion
    lightVal = analogRead(lightPin);//check ligght intensity
    Blynk.virtualWrite(V3, pir_val);
    if (pinValue == 1) {//if LED is initially on
      if (lightVal > 750) { // turn off because room is already bright
        colorLoop(0, 0, 0); //turn off
        Blynk.virtualWrite(V1, 0);//turn blynk app button off
        Blynk.notify("Bright Room : turning LED OFF-Saving 3.6W");
        pinValue = 0;
      }
      else if (lightVal < 300 && pir_val == 0) {
        //        finishnomtion++;//
        //nomotiontime = finishnomtion - startmotion; //calculate time of no motion
        //        Serial.print("no Motion time : ");
        //        Serial.println(nomotiontime);
        colorLoop(0, 0, 0); //turn off
        Blynk.virtualWrite(V1, 0);//turn blynk app button off
        Blynk.notify("No Motion : turning LED OFF-Saving 3.6W");
        pinValue = 0;
        //          startmotion = 0;//start the timing at motion detecting
        //          startmotion++;
        //          finishnomtion = 0;
      }
      else if (lightVal < 300 && pir_val == 1) {//reset timer
        //          nomotiontime = 0;
        //          startmotion = 0;//start the timing at motion detecting
        //          startmotion++;
        //          finishnomtion = 0;
      }
    }
    else {// if LED is intitally off
      if (lightVal < 300 && pir_val == 1) {
        colorLoop(r, g, b); // turn on
        Blynk.virtualWrite(V1, 1); //turn led buttion on
        Blynk.notify("Motion Detected : turning LED ON");
        pinValue = 1;
      }
    }
  }
  else {
    Blynk.virtualWrite(V3, 0);
  }
}


BLYNK_WRITE(V4)// turn on smart lighting
{
  smartLighting = param.asInt();
}

BLYNK_WRITE(V5) {
  sendSetting = param.asInt();
  if (sendSetting == 1) {
    //    //saveColor
    colorPicker(colorChoice, rSetting, gSetting, bSetting);
    redArr[lightingIndex] = rSetting;
    greenArr[lightingIndex] = gSetting;
    blueArr[lightingIndex] = bSetting;
    //save brightness
     brightArr[lightingIndex] = brightSetting;
    //    //save name
        strcpy(options[lightingIndex], l_name.c_str());
        Blynk.setProperty(V8, "labels", options[0], options[1], options[2], options[3], options[4], options[5], options[6], options[7], options[8], options[9]);
        Blynk.notify("Light Setting received");
    Blynk.virtualWrite(V5, 0);
    sendSetting = 0;
  }
}


BLYNK_WRITE(V6)// color Setter
{
  colorChoice = param.asInt();

}
BLYNK_WRITE(V7) {//Turn on specific lighting
  turnOnLighting = param.asInt();
  if (turnOnLighting == 1) {
    FastLED.setBrightness(brightArr[lightSetting]);
    colorLoop(redArr[lightSetting], greenArr[lightSetting], blueArr[lightSetting]);
    Blynk.notify("Specific Light Settings on");
  }
  else if (pinValue == 0 && turnOnLighting == 0) {
    colorLoop(0, 0, 0); // off
  }
}
BLYNK_WRITE(V8) {//which lightSetting we want to turn set
  lightSetting = param.asInt();
  lightSetting = lightSetting - 1;
}
BLYNK_WRITE(V9) {//light settings option name
  l_name = param.asStr();
  //optionName = l_name.c_str();
}

BLYNK_WRITE(V10) {//light settings option index
  lightingIndex = param.asInt();
  lightingIndex = lightingIndex - 1;
}
BLYNK_WRITE(V11) {//light settings brightness
  brightSetting = param.asInt();
}

void setup()
{
  // Debug console
  //Serial.begin(9600);
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
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}
void colorLoop(uint8_t red, uint8_t green, uint8_t blue) {
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(green, red, blue);
    FastLED.show();
  }
}
