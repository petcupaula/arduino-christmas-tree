/* 
 Script for controlling an LED and relay based on clapping detected by a mic.
 The relay can be connected to a power cord and can thus control existing light setups. 
 
 Turns on/off the lights when clapping twice.
 
 Components:
 * Arduino Uno
 * Relay
 * Mic
 * LED
*/

// mic
#define MicPin A0
const int sampleWindow = 6; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

// clapping
int clapWindow = 20;
int clap = 0;
double clapThreshold = 1.5;

// led
int led = 9;           // the PWM pin the LED is attached to
bool lights_on = false;

// relay
int relay_pin = 8;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  lights_on = false;

  pinMode(relay_pin,OUTPUT);
  digitalWrite(relay_pin, HIGH);
  
  Serial.begin(9600);
}

void loop() {

  unsigned long startMillis= millis();
  while (millis() - startMillis < clapWindow) {
  
    double volts = readMic();
  
    if (volts > clapThreshold) {
      if (clap == 0) {
          clap++;
        }
      else if (clap == 1) {
        // 2 claps!!
        manageLights();
        clap = 0;
      }
    }
  }

  clap = 0;
  
}


double readMic() {
     unsigned long startMillis= millis();  // Start of sample window
     unsigned int peakToPeak = 0;   // peak-to-peak level
  
     unsigned int signalMax = 0;
     unsigned int signalMin = 1024;
     // collect data for sampleWindow mS
     while (millis() - startMillis < sampleWindow)
     {
        sample = analogRead(MicPin);
        if (sample < 1024)  // toss out spurious readings
        {
           if (sample > signalMax)
           {
              signalMax = sample;  // save just the max levels
           }
           else if (sample < signalMin)
           {
              signalMin = sample;  // save just the min levels
           }
        }
     }
     peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
     double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
     Serial.println(volts);
     return volts;

}

void manageLights() {
  if (lights_on) {
    // turn off
    digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
    digitalWrite(relay_pin, HIGH);
    lights_on = false;
    }
  else {
    // turn on
    digitalWrite(led, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(relay_pin, LOW);
    lights_on = true;
  }
}
