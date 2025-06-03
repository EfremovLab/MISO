#include <Servo.h>

/*
 * Plunger Control 
*/
// PINS DEFINED
const int Pin_PUL = 6;
const int Pin_DIR = 7; 
const int Pin_STEPPER = 5; // CONNECT WIRE TO CONTROL POWER OF STEPPER TO PIN 5
const int Pin_BUTTON = 2; 
const int Pin_Magnet = 4;
const int Pin_JOY_UP=11;
const int Pin_JOY_DOWN= 13;
const int Pin_JOY_LEFT=8;
const int Pin_JOY_RIGHT=10;
const int SERVO_MOTOR=9; 

// Servo and stepper  motor variables
float Rotation_angle; 
float servo_angle=98;
//float Resting_angle=90;
float Blotting_angle=5;
int delay_time=1000;
Servo servoMotor;  // Create Servo Motor object
int stepper_position=0;

// TRIGGERS and logical switches
byte      inFlag = 0;
bool      StepperOn   = false;
bool      StepperOff  = false;
bool      auto_plunge = true;
String    inString=""; 
char      inChar;
bool      magnet_on   =false; 
bool button = false;
volatile bool button_detected=false ;
unsigned long button_time;
int button_count=0;  

bool AutoSpreadAndPlunge = false;

unsigned long count ; 

//const int n = 3; // number of full times (up and down together) the sample is drawn on the grid

/*
 Temperature Control
 */
const int Temp_inPIN  = A0;
//const int PIN_hum = A5; // Humidity sensor powered by the digital output from DAQ (from port 3)
//int hum;
//float RH;
const int Heat_powerPIN = 12;
float     Tset = -173; // target temperature
float       Temperature; 
int       TempRaw;  //read out form Termometer pin 5V = 1023
unsigned long  Nmeas1 = 0;
unsigned long  Nmeas2 = 0;
float          IntP = 0; //short integral for proportional signal
float          IntI = 0; // integarl of signal over integration time 
float          I = 0 ;
float          P=  0;
unsigned long  refTime;
int            Tint = 5; // integration time in sec
int            Tprop ; 
int            OutPower =0 ; // output power
float          Kp = 10; // proportional coefficient
float          Ki = 2; // Integral coefficient 
float          DT;
int            Iconstr; 

int hum;
float RH;
const int Pin_hum = A1;


// the setup function runs once when you press reset or power the board
void setup() {
  servoMotor.attach(SERVO_MOTOR);
  // initialize digital pin LED_BUILTIN as an output.
pinMode(Pin_JOY_UP, INPUT);
pinMode(Pin_JOY_DOWN, INPUT);
pinMode(Pin_JOY_LEFT, INPUT);
pinMode(Pin_JOY_RIGHT, INPUT);
servoMotor.write(servo_angle);// initialize servo to the middle

  pinMode(Pin_PUL, OUTPUT);
  pinMode(Pin_DIR, OUTPUT);
  pinMode(Pin_STEPPER, OUTPUT);
  pinMode(Pin_BUTTON, INPUT);
  pinMode(Pin_Magnet, OUTPUT);
  
  digitalWrite(Pin_DIR, LOW);
  digitalWrite(Pin_STEPPER, LOW);
  digitalWrite(Pin_Magnet, LOW);
  digitalWrite(Pin_PUL, LOW);
  digitalWrite(Pin_BUTTON, LOW);
  
  attachInterrupt(digitalPinToInterrupt(Pin_BUTTON), BUTTONpush, RISING);

  Serial.begin(9600);

  // put your setup code here, to run once:
  pinMode(Temp_inPIN, INPUT);
  pinMode(Heat_powerPIN, OUTPUT);
  analogWrite(Heat_powerPIN, 0);
  refTime = millis();
  Tint *= 1000;
  Tprop = Tint - 1000; // integrate over 1 sec
  Iconstr = int(255/Ki)+1;

  //ACTIVATE INTERRUPT
}

 void BUTTONpush()
 {
   button_detected = true;
   button_time=micros();
 }

float measHUM() {
  hum = analogRead(Pin_hum);
  return (5.0/1024*hum - 0.836)/0.03;

}
void blot_routine()
{
  servo_angle=servoMotor.read();
  servoMotor.write(servo_angle-Blotting_angle);
  delay(delay_time);
  servoMotor.write(servo_angle+2);
  delay(200);
  servoMotor.write(servo_angle);
}
void move_grid(int nr_steps, int mot_delay ) // Direction positive - up, negative - down; nr_steps - number of steps to walk; mot_delay step delay in microseconds
{
  if (nr_steps > 0){
    digitalWrite(Pin_DIR, HIGH);
    stepper_position = stepper_position + nr_steps;
    }
  else
  {
    digitalWrite(Pin_DIR, LOW);
    stepper_position = stepper_position - nr_steps;
    nr_steps = nr_steps * (-1);
  }
  count = 0;
  while(count < nr_steps) {
    count++;
    digitalWrite(Pin_PUL, HIGH);   
    delayMicroseconds(mot_delay);                       
    digitalWrite(Pin_PUL, LOW);    
    delayMicroseconds(mot_delay);
    }
  //Serial.println("stepper position "+ String(stepper_position) );
}


void readSerial() {
  while (Serial.available() > 0) {
    byte inChar = ((byte)Serial.read());
//  
    if (isDigit(inChar) or (inChar == 46) or (inChar == 45)  )  { inString += char (inChar);} // 46 ASCII code for '.' 45 '-' 
    
    if (inChar == 'r') {inFlag = 2;} // resets the plunger
    else if (inChar == 'z') {auto_plunge = not(auto_plunge);} // Toggles automated not automated plunging
//  else if (inChar == 'c') { Trigger3 = false;} // Closes the plunging arm
    else if (inChar == 'h') {inFlag = 4;} // Moves the grid in the High Direction (Up), after turning on the stepper motor driver
    else if (inChar == 'l') {inFlag = 5;} // Moves the grid in the Low Direction (Down), after turning on the stepper motor driver
    else if (inChar == 'd') {inFlag = 6;} // starts writing in downward direction on the grid
    else if (inChar == 'u') {inFlag = 7;} //starts writing in upward direction on the grid
    else if (inChar == 'p') {inFlag = 8; AutoSpreadAndPlunge = true;} // Plunges the grid in liquid ethane
    else if (inChar == 'x') {inFlag = 9;} //Moves the grid faster in the High Direction (Up), after turning on the stepper motor driver
    else if (inChar == 'y') {inFlag = 10;} //Moves the grid fastre in the Low Direction (Down), after turning on the stepper motor driver
    else if (inChar == 'g') {inFlag = 11;} // Is true when the grid adjustment is complete
    else if (inChar == 'a') {inFlag =12; } // a - rotation angle initialized at 90 
    //else if (inChar == 'i') {inFlag =13; } // i -inside angle position
    else if (inChar == 'b') {inFlag =14; } // b - blotting angle
    else if (inChar == 't') {inFlag =15; } // t - delay time
    else if (inChar == 's') {inFlag =16; } // s - start blotting

    else if (inChar == 'n') {StepperOn = true;} // Turns on the stepper motor driver
    else if (inChar == 'f') {StepperOff = true;} // Turns off the stepper motor driver

    if (  inChar == 10  )  // 10 -ASCII for ENTER assign entered numbers to variables and define limits of the variables
    {
      if ( inFlag == 12 )  {
        servo_angle = constrain( inString.toFloat(), 0, 180);
        //Serial.println(servo_angle);
        servoMotor.write(servo_angle);
      }
    /*else if ( inFlag == 13 )  // restign angle = inside angle
    {
      Resting_angle = constrain( inString.toFloat(), 0, 180);
      Serial.println("resing angle set to "+String( Resting_angle));
    }
    */
    else if ( inFlag == 14 )  // blotting angle  
    {
      Blotting_angle = constrain( inString.toFloat(), 0, 180);
      //Serial.println("blotting angle set to "+ String(Blotting_angle) );

    }
    else if ( inFlag == 15 )  // delay time  
    {
      delay_time = constrain( inString.toFloat(), 0, 10000);
      //Serial.println("blotting time  set to "+ String( delay_time)+ " ms");
    }
    inString=""; // reset read number after every ENTER 
    }  
  }

}


// the loop function runs over and over again forever
void loop() {

// check is button has been pressed - danger the delay time at teh end of teh cycle 
if ((button_detected == true) and (micros()-button_time > 20000) and (digitalRead(Pin_BUTTON)== HIGH)){
 button_detected=false;
 button = true;
 if (button_count == 0){button_count = 1;}
 else if (button_count == 1){button_count = 0;}
//Serial.print(button);
//Serial.print(",");
}
  
readSerial(); // check if serial input contains data and reads them, sets up inFlag StepperOn flag and assigns variables


// ********************************************** PLUNGER ARM CONTROL *****************************************
// *1   Turn On/OFF the stepper motor
if (StepperOn == true ) {
  StepperOn = false;
 digitalWrite(Pin_STEPPER, HIGH);
 move_grid(130, 500);
 move_grid(-20, 500);
}

else if (StepperOff == true) {
  StepperOff = false;
  digitalWrite(Pin_STEPPER, LOW); 
}
// *2 Moves grid one step up with 10 ms delay time 
if (inFlag == 4) {
move_grid(1, 10000);
inFlag=0;
}

// *3 MOVES grid by 10 steps up with 2 ms delay times in between steps
if (inFlag == 9) {
digitalWrite(Pin_DIR, HIGH);
move_grid(10, 2000);
inFlag=0;
}

// *4 One step DOWN with 10 ms delay time 
if (inFlag == 5 ) {
move_grid(-1, 10000);
inFlag=0;
}
// *5  10 Steps down with 2 ms delay in between
if (inFlag == 10 ) {
move_grid(-10, 2000);  
inFlag = 0;
}
// *6 25 steps up with 10 ms delay 
if (inFlag == 6 ){
  inFlag = 0;
  move_grid(25, 10000);
}
// *7 25 steps down with 10 ms delay time
if (inFlag == 7 ){
  inFlag = 0;
  move_grid(-25, 10000);
}
// *8 Upon button pressing shifts grid down for blotting 100 steps 1 ms delay time
if (button == true and button_count == 1 and not(auto_plunge)){
  button = false;
  move_grid(-100, 1000);
} 

// *9 Plunges the grid in liquid ethane 2150 steps 10 us in between very fast plunging
  if ( button == true and button_count == 0 and not (auto_plunge)){
  button = false;
  move_grid(-2150, 10);
  } 
//*10 Moves arm back to close to capillary position  and turns off the motor? 
  if ( inFlag == 2){
    inFlag = 0;
    move_grid(2220, 500);
  }
//* 11 Automated plunging procedure
if (auto_plunge and button )
{
  button = false;
  move_grid(-400, 1000);
  blot_routine();
  delay(100);
  move_grid(-1750, 10);
}   

//* 11 Automated plunging procedure with the char from labview instead of button
if (auto_plunge and AutoSpreadAndPlunge )
{
  AutoSpreadAndPlunge = false;
  button = false;
  move_grid(-350, 1000);
  blot_routine();
  delay(100);
  move_grid(-1800, 10);
}   

// *12 JOYSTICK
if (digitalRead(Pin_JOY_UP) == HIGH)    {Serial.println("UP");  move_grid(5, 500);}
if (digitalRead(Pin_JOY_DOWN) == HIGH)  {Serial.println("DOWN"); move_grid(-5, 500);}
if (digitalRead(Pin_JOY_LEFT) == HIGH)  {
  //Serial.println("LEFT"); 
  servo_angle=servoMotor.read();
  servo_angle= servo_angle + 1; 
  servoMotor.write(servo_angle);
  //Serial.println("servo anfle "+ String(servo_angle) );
  delay(500);
} 
if (digitalRead(Pin_JOY_RIGHT) == HIGH) {
  //Serial.println("RIGHT");
  servo_angle=servoMotor.read();
  servo_angle= servo_angle - 1; 
  servoMotor.write(servo_angle);
  //Serial.println("servo anfle "+ String(servo_angle) );
  delay(500);
}

// ************************************************ END PLUNGER ARM CONTROL **************************

if (inFlag == 16){
  inFlag = 0;
  blot_routine();
}


// ************************************************ HUMIDITY MEASUREMENT **************************


RH = measHUM()-14.0;
Serial.print(String(RH));
Serial.print(",");

// ********************************************************* TEMPERATURE CONTROL*************************
TempRaw=analogRead(Temp_inPIN);
if (TempRaw > 328 ){
  Temperature = 0.2056 * TempRaw - 209.66;
}
else { 
  Temperature = 0.4608 * TempRaw - 299;
}

//Serial.print ("\n Raw data  ");
//Serial.print(TempRaw);
//Serial.print ("  Reaculculated temperature:");
Serial.println ( Temperature );
//Serial.write(char(Temperature));
//Serial.print ("\n Output power  ");
//Serial.print(OutPower);
//Serial.print (" DT  ");
//Serial.print(DT);
//Serial.print (" INTEGRATED power  ");
//Serial.print(Ki*I);

DT = Tset-Temperature;
//PID control Integration time is around 10-20 sec
//constrain for negative values??? Needed???
if ( (millis() - refTime) < Tint ) { 
  Nmeas1++;
  IntI += DT;
  if( (millis() - refTime) > Tprop ) {
  Nmeas2++;
  IntP += (DT);
  }
 }
else {
  I += (IntI/Nmeas1);
  I =  constrain (I, 0, Iconstr);
  P = IntP/Nmeas2;
  Nmeas1 = 0;
  Nmeas2 = 0;
  refTime = millis();
  IntI = 0; 
  IntP = 0;
  OutPower = constrain (int(Kp*P + Ki*I), 0, 255);
  if ( DT < -2 ) {OutPower = 0; } //overshoot protection
  analogWrite(Heat_powerPIN, OutPower);
//  digitalWrite(Pin_LED, !digitalRead(Pin_LED));
}

// Humidity measurement ( from teh calibration chart of HIH 4000 series at 5 V and 25C
//hum = analogRead(PIN_hum);
//RH = (5.0/1024*hum - 0.826)/0.0315;
//Serial.println("Humidity: " + String(RH) + "%");
// ************************************************END TEMPARATURE CONTROL ********************
//RH = measHUM()-14.0;
//Serial.println(String(RH));


delay (100);

}
