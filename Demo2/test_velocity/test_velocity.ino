//this code is forward position control ONLY

#define PI 3.1415926535897932384626433832795 //definition for PI constant

//Pin assignment and global variable instantiation
const int powerPin = 11; //pin for an extra Vcc = 5V
const int channelA1Pin = 2;
const int channelB1Pin = 5;
const int channelA2Pin = 3;
const int channelB2Pin = 6;

const int enablePin = 4; //Pin for enabling motor shield
const int voltageMotor1 = 9; //Pin for voltage given to motor 1
const int voltageMotor2 = 10; //Pin for voltage given to motor 2 (not used)
const int signMotor1 = 7; //Pin for direction of motor 1
const int signMotor2 = 8; //Pin for direction of motor 2 (not used)
const int statusFlag = 12; //Pin for status flag

//ISR variables
volatile bool channelA1 = HIGH; //Channel A1 set on default
volatile bool channelB1 = HIGH; //Channel B1 set on default
volatile bool channelA2 = HIGH; //Channel A2 set on default
volatile bool channelB2 = HIGH; //Channel B2 set on default
volatile int count1 = 0; //counter variable for position
volatile int count2 = 0; //counter variable for position

//main loop variables
unsigned long timeDelta = 0; //time in between loops
unsigned long timeMain = 0; //time loop takes place
float velocity1 = 0; //velocity of left wheel
float velocity2 = 0; //velocity of right wheel
float velocityForward = 0; //forward velocity of robot
float velocityAngular = 0; //angular velocity of robot
float positionForward = 0; //forward position of robot
float positionAngular = 0; //angular position of robot

float desiredVelocity = 7.4;

float errorLeftMotor = 0;
float desiredLeftMotor = desiredVelocity;
float integralLeftMotor = 0;
float KpLeftMotor = 7;
float KiLeftMotor = 0;

float errorRightMotor = 0;
float desiredRightMotor = desiredVelocity;
float integralRightMotor = 0;
float KpRightMotor = 7;
float KiRightMotor = 0;

int PWM = 0; //ideal PWM
int OldPWM = 0; //PWM of last iteration
int PWM1 = 0; //PWM for left wheel
int PWM2 = 0; //PWM for right wheel

void setup() { //Sets up pins and serial monitor
  Serial.begin(9600); //open serial
  pinMode(channelA1Pin,INPUT_PULLUP);
  pinMode(channelB1Pin,INPUT_PULLUP);
  pinMode(channelA2Pin,INPUT_PULLUP);
  pinMode(channelB2Pin,INPUT_PULLUP);

  pinMode(voltageMotor1,OUTPUT);
  pinMode(voltageMotor2,OUTPUT);
  pinMode(signMotor1,OUTPUT);
  pinMode(signMotor2,OUTPUT);
  pinMode(statusFlag,INPUT);
  pinMode(enablePin,OUTPUT);
  digitalWrite(enablePin,HIGH);
  
  pinMode(powerPin,OUTPUT);
  digitalWrite(powerPin,HIGH);
  attachInterrupt(digitalPinToInterrupt(channelA1Pin), encoder1ISR, CHANGE); //sets interrupt to happen when channel A1 changes
  attachInterrupt(digitalPinToInterrupt(channelA2Pin), encoder2ISR, CHANGE); //sets interrupt to happen when channel A2 changes
}

void loop() { //main loop (nothing)
  timeDelta = micros()-timeMain; //time in between loops
  timeMain = micros(); //time of current loop

  velocity1 = (count1*2*PI*1000000.0)/(3200.0*timeDelta); //rotational velocity of wheel 1
  count1=0; //resets counts of wheel 1
  velocity2 = (count2*2*PI*1000000.0)/(3200.0*timeDelta); //rotational velocity of wheel 2
  count2=0; //resets counts of wheel 1
  velocityForward = (velocity1+velocity2)*(3.0/2.0); //forward velocity of robot
  velocityAngular = (velocity1-velocity2)*(3.0/11.0); //angular velocity of robot
  positionForward = positionForward+(velocityForward * timeDelta/1000000.0); //forward position of robot
  positionAngular = positionAngular+(velocityAngular * timeDelta/1000000.0); //angular position of robot

  digitalWrite(signMotor1,HIGH);
  digitalWrite(signMotor2,LOW);

  errorLeftMotor = (desiredLeftMotor - velocity1);
  integralLeftMotor = integralLeftMotor + (errorLeftMotor * timeDelta / 1000000.0); //integral path (rad)
  errorLeftMotor = (KpLeftMotor*errorLeftMotor)+(KiLeftMotor*integralLeftMotor); //proportional path (volts)
  PWM1 = PWM1+int(errorLeftMotor*17);
  if(PWM1>255){ //saturates PWM and caps at 255
    PWM1 = 255;
  } else if (PWM1<-255){
      PWM1 = -255;
  }

  errorRightMotor = (desiredRightMotor - velocity2);
  integralRightMotor = integralRightMotor + (errorRightMotor * timeDelta / 1000000.0); //integral path (rad)
  errorRightMotor = (KpRightMotor*errorRightMotor)+(KiRightMotor*integralRightMotor); //proportional path (volts)
  PWM2 = PWM2+int(errorRightMotor*17);
  if(PWM2>255){ //saturates PWM and caps at 255
    PWM2 = 255;
  } else if (PWM2<-255){
    PWM2 = -255;
  }

  /*
  if ((PWM-OldPWM) > 70) { //slows how much PWM can increase by 2v
    PWM = OldPWM+70;
  }
  if ((PWM-OldPWM) < -70){ //slows how much PWM can decrease by 2v 
    PWM = OldPWM-70;
  }
  */

  analogWrite(voltageMotor1,abs(PWM1)); //writes PWM counts to motor 1
  analogWrite(voltageMotor2,abs(PWM2)); //writes PWM counts to motor 2 

  //prints out data for debugging (optional)
  Serial.print("velocity1: ");
  Serial.print(velocity1);
  Serial.print("\tvelocity2: ");
  Serial.print(velocity2);
  Serial.print("\tvelocityForward: ");
  Serial.print(velocityForward);
  Serial.print("\tvelocityAngular: ");
  Serial.println(velocityAngular);
  Serial.print("positionForward: ");
  Serial.print(positionForward);
  Serial.print("\tpositionAngular: ");
  Serial.println(positionAngular);
  Serial.println();
}

void encoder1ISR() { //interrupt for wheel 1 (left)
  channelA1 = digitalRead(channelA1Pin); //Reads channels for current iteration
  channelB1 = digitalRead(channelB1Pin);
  if (channelA1==channelB1){ //Adds to counter if channel A matches channel B
      count1=count1-2;      
  } else { //Subtracts from counter if channel A differs from channel B
      count1=count1+2;
  }
}

void encoder2ISR() { //interrupt for wheel 2 (right)
  channelA2 = digitalRead(channelA2Pin); //Reads channels for current iteration
  channelB2 = digitalRead(channelB2Pin);
  if (channelA2==channelB2){ //Adds to counter if channel A matches channel B
      count2=count2-2;      
  } else { //Subtracts from counter if channel A differs from channel B
      count2=count2+2;
  }
}
