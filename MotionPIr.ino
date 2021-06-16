
// add led pin
int inputPin = 7;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
 
void setup() {
  pinMode(inputPin, INPUT);     // declare sensor as input
 //add led setup
  Serial.begin(115200);
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  
  if (val == HIGH) // check if the input is HIGH
  {            
    //turn led On
    if (pirState == LOW) 
  {
      Serial.println("Motion detected!"); // print on output change
      pirState = HIGH;
    }
  } 
  else 
  {
    //turn led off
    if (pirState == HIGH)
  {
      Serial.println("Motion ended!");  // print on output change
      pirState = LOW;
    }
  }
}
