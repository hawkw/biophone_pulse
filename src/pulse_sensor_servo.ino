#include <Arduino.h>

#include <Servo.h>
#include <known_16bit_timers.h>

#include <Wire.h>;
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ----------------------
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 13
2) Fades an LED to User's Live HeartBeat
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
 ----------------------       ----------------------  ----------------------
*/



//const int NUMPIXELS = 14;           // Put the number of NeoPixels you are using here
Servo servo;  // create servo object to control a servo
const int servo_theta = 30
        , servo_zero = -30
        , servo_pin = 3;
//  Variables
const int pulsePin = 0            // Pulse Sensor purple wire connected to analog pin 0
        , blinkPin = 13                // pin to blink led at each beat
        , fadePin = 5;                  // pin to do fancy classy fading blink at each beat

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
int fadeRate = 255;                 // used to fade LED on with PWM on fadePin
int pos;
// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse


//rgb_color colors[NUMPIXELS];
void setup(){
  Wire.begin(); // join i2c bus (address optional for master)
//  ledStrip.interruptFriendly = true;
//  for (uint16_t i = 0; i < NUMPIXELS; i++) {
//    colors[i] = rgb_color(255, 0, 0);
//  }
//  ledStrip.write(colors, fadeRate);
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  analogWrite(fadePin, fadeRate);

  servo.attach(servo_pin);
  servo.write(servo_zero);
  delay(1000);                      // Wait a second
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
   // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
   // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
//   analogReference(EXTERNAL);



}


//  Where the Magic Happens
void loop(){
    // send BPM down the I2C wire
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(BPM);              // sends one byte
    Wire.endTransmission();    // stop transmitting

    serialOutput() ;
    // A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
  if (QS == true){
       fadeRate = 255;         // Makes the LED Fade Effect Happen

        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.

        // beat the drum
        servo.write(servo_theta);
        delay(35);

        QS = false;                      // reset the Quantified Self flag for next time
  }
//  delay(20);
//
  ledFadeToBeat();
  delay(20);
  servo.write(servo_zero);
  // Makes the LED Fade Effect Happen


}

void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
}
//
//
//
