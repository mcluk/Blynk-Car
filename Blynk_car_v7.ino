/*  FIX TO WEMOS MOTOR SHIELD LIBRARY EXAMPLE
 *   
 *   v1 2017 JAN 06 FIX: 
 *   
 *   Objects M1 and M2 should be defined dynamically!
 *   When defined statically the class constructor starts before before setup-method.
 *   So, when executing constructor of Motor class the Serial port is not initialized yet!
 *   https://forum.wemos.cc/topic/841/not-sure-if-i-connected-the-motor-shield-correctly/20
 *   
 *   v2 Add Blynk integration and motor working
 *   
 *   v2 FIX: Moved Blynk.run() to beginning of main loop 
 *   
 *   v3 FIX: Convert synchronous motor commands to asynchronous
 *   at the same time incorporating joystick feedback from Blynk.
 *   Package forward back, left right into motor functions that
 *   converts acceleration into a velocity slider.
 *   
 *   Remove OTA
 *   
 *   v4 FIX: Rebuild app with values to 1023 only (vs 1024 in v3)
 *   http://community.blynk.cc/t/wemos-d1-all-time-disconnecting/9417/17
 *   
 *   Staggering interrupts did NOT work either though it was much more responsive.
 *   
 *   v5 FIX: Move from 2 joysticks to 1 (still disconnecting)
 *   So removed both display value widgets which STOPPED constant disconnects.
 *   But motors do not turn although serial monitor shows it should work
 *   
 *   v6 continuous crashing and reconnecting with exception (28)
 *   Not sure what I did to break it. Cut & paste into and from text editor did not work.
 *   
 *   v7 converted over to WeMos and WORKS!!!!
 *   Moved run motors if statements to BEFORE Blynk.run() in main loop. 
 *   Note that moving if statements into a procedure runMotors() breaks code
 *   and motors stop spinning.
 *   http://community.blynk.cc/t/robot-with-esp8266-shield-and-arduino-nano/3316
 */

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

//#include <ArduinoOTA.h>       // for local OTA updates
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include <SimpleTimer.h>      // Essential for almost all sketches
//SimpleTimer timer;

//********************* COMPLETE WITH YOUR DETAILS *************
// Get Auth Token in the Blynk App.
char auth[] = "8d459f8f40964994954a0d2fa4972e63";  // on Android for iot car project
//char cloudserver[16] = "blynk-cloud.com";
//char localserver[16] = "xxx.xxx.xxx.xxx";  // IP details for the local server.
char ssid[] = "CEREBRO_GUEST";               // Your WiFi credentials.
char pass[] = "17flyonWater";                // Password.
//char OTAhost[] = "iot car";              // Optional.
//****************

#include <WEMOS_Motor.h>

int motorA ; // power level to motor A
int motorB ; // power level to motor B
int X=0;
int Y=0;
int factor=0; // Variable to reduce speed of the motorA or B and turn.
int maximo=70;

//Motor shiled I2C Address: 0x30
//PWM frequency: 1000Hz(1kHz)
Motor * M1; //(0x30,_MOTOR_A, 1000);//Motor A
Motor * M2; //(0x30,_MOTOR_B, 1000);//Motor B

void setup()
{
  // Set console baud rate
  Serial.begin(9600);
  delay(10);

  Serial.println("\n Starting");
  Blynk.begin(auth, ssid, pass);               // normal Blynk Cloud server connection     

  M1 = new Motor(0x30,_MOTOR_A, 1000);//Motor A 
  M2 = new Motor(0x30,_MOTOR_B, 1000);//Motor B 

}

 BLYNK_WRITE(V1) 
{
 int X1 = param.asInt();
 X=X1;
 Serial.printf("V1  X = %d   Y = %d\r\n", X, Y);
}

 BLYNK_WRITE(V2)
{
 int Y1 = param.asInt();
 Y=Y1;
 Serial.printf("V2  X = %d   Y = %d\r\n", X, Y);
}

 BLYNK_WRITE(V0)//      slider  from 100 to 255
{
 int vel = param.asInt(); 
 maximo=vel;
 Serial.printf("V0 slider = %d\r\n", maximo);
 }

void loop()
{

  if(X == 128  &&  Y == 128)  //  Stop
  {
   motorA = 0;
   motorB = 0;
  M1->setmotor(_STOP);
  M2->setmotor( _STOP);
  Serial.println("Motor A&B STOP");
   } 
   
   if(X >= 129 && Y == 128)   // Forward
  {
    motorA = X;
    motorB = X;
    
    motorA = map(motorA, 129,255 , 70,maximo);
    motorB = map(motorB, 129,255 , 70,maximo);
    M1->setmotor( _CW, motorA);
    M2->setmotor( _CW, motorB);
    Serial.printf("A:%d%, B:%d%, DIR:CW FORWARD\r\n", motorA,motorB);
  }
   if(X >= 129 && Y <= 127)   // Turning right forward 
  {
    motorA = X;
    motorB = X;
    factor = Y;
    factor= map(factor,0,511,40,0);
    
    motorA = map(motorA, 129,255 , 70,maximo);
    motorB = map(motorB, 129,255 , 70,maximo);
    M1->setmotor( _CW, motorA);
    M2->setmotor( _CW, (motorB-factor));
    Serial.printf("A:%d%, B:%d%, DIR:CW FORW RIGHT\r\n", motorA,motorB-factor);
  }
   if(X >= 129 && Y >= 129)   // Turning left forward 
  {
    motorA = X;
    motorB = X;
    factor = Y;
    factor= map(factor,129,255, 0,35);
    
    motorA = map(motorA, 129,255 , 70,maximo);
    motorB = map(motorB, 129,255 , 70,maximo);
    M1->setmotor( _CW, (motorA-factor));
    M2->setmotor( _CW, motorB);
    Serial.printf("A:%d%, B:%d%, DIR:CW FORW LEFT\r\n", motorA-factor,motorB);
  }
  if(X <= 127 && Y ==128)   // Reverse
  {
    motorA = X;
    motorB = X;
    
    motorA = map(motorA, 0,126 , maximo,70);
    motorB = map(motorB, 0,126 , maximo,70);
    M1->setmotor(_CCW, motorA);
    M2->setmotor(_CCW, motorB);
    Serial.printf("A:%d%, B:%d%, DIR:CCW REVERSE\r\n", motorA,motorB);
  }
  if(X <= 127 && Y <=127)   // Turning right in reverse 
  {
    motorA = X;
    motorB = X;
    factor = Y;
    factor= map(factor,0,127, 35,0);
    
    motorA = map(motorA, 0,126 , maximo,70);
    motorB = map(motorB, 0,126 , maximo,70);
    M1->setmotor( _CCW, motorA);
    M2->setmotor( _CCW, (motorB-factor));
    Serial.printf("A:%d%, B:%d%, DIR:CCW REV RIGHT\r\n", motorA,motorB-factor);
  }
  if(X <= 127 && Y >=129)   // Turning left in reverse 
  {
    motorA = X;
    motorB = X;
    factor = Y;
    factor= map(factor,129,255, 0,35);
    
    motorA = map(motorA, 0,126 , maximo,70);
    motorB = map(motorB, 0,126 , maximo,70);
    M1->setmotor( _CCW, (motorA-factor));
    M2->setmotor( _CCW, motorB);
    Serial.printf("A:%d%, B:%d%, DIR:CCW REV LEFT\r\n", motorA-factor,motorB);
  }
  
  Blynk.run();
}
