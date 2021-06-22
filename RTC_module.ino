#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DS3231 clock;
RTCDateTime dt;

void setup()
{
  Serial.begin(9600); //sets up serial monitor
  lcd.begin(16,2);
  Serial.println("Initialize RTC module");
  // Initialize DS3231
  clock.begin();

  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);    //sets the date and time
  /*
  Tips:This command will be executed every time when Arduino restarts. 
       Comment this line out to store the memory of DS3231 module
  */
}

void loop()
{
  dt = clock.getDateTime();

  // For leading zero look to DS3231_dateformat example
  lcd.setCursor(0,0);
  lcd.print("Hello! Today is: ");
  
  lcd.setCursor(0,1);
  //lcd.scrollDisplayLeft(); 
  lcd.print(dt.month);  lcd.print("-");
  lcd.print(dt.day);    lcd.print("-");
  lcd.print(dt.year);   lcd.print(" ");
  //lcd.print("And it is currently ");
  lcd.print(dt.hour);   lcd.print(":");
  lcd.print(dt.minute); 
  
  //lcd.print(":");
 // lcd.print(dt.second); lcd.println("");
  
  //Serial.print("Hello! Today is ");
  //Serial.print(dt.month);  Serial.print("-");
  //Serial.print(dt.day);    Serial.print("-");
  //Serial.print(dt.year);   Serial.print(" ");
  //Serial.print("And it is currently ");
  //Serial.print(dt.hour);   Serial.print(":");
  //Serial.print(dt.minute); Serial.print(":");
  //Serial.print(dt.second); Serial.println("");

  

  delay(1000);
}
