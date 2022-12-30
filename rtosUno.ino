#include <Wire.h>

#include <Arduino_FreeRTOS.h>

int motor1pin1 = 2;
int motor1pin2 = 3;//a0

int motor2pin1 = 4;
int motor2pin2 = 5;//a1

int rightIRpin = 6;
int leftIRpin = 7;

int carSpeed = 100;
int fastCarSpeed = 200;
char command; 

int lightSensorPin = 8;
int ledPin =9;

void setup() {

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT); 

  pinMode(rightIRpin, INPUT); 
  pinMode(leftIRpin, INPUT); 

  pinMode(lightSensorPin,INPUT);
  pinMode(ledPin,OUTPUT);


  
  // set segment pins as OUTPUT  
  Serial.begin(9600); 
      // Wire.begin();
  xTaskCreate(rcCar,"rcCar",128,NULL,1,NULL);


  vTaskStartScheduler();

}

void loop() {


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void rcCar(void*pvParameters){

  while(1){
        
      int lightSensorReading=digitalRead(lightSensorPin);      //assign value of LDR sensor to a temporary variable
      if(((digitalRead(rightIRpin) == 1)&&(digitalRead(leftIRpin) == 0))){//if Right Sensor is Black and Left Sensor is White then it will call turn Left function
          fastLeft();
          delay(250);
        }
        else{
          if(((digitalRead(rightIRpin) == 0)&&(digitalRead(leftIRpin) == 1))){//if Right Sensor is White and Left Sensor is Black then it will call turn Right function

            fastRight();
            delay(250);  
          }
      }


    if(Serial.available() > 0){
      command = Serial.read(); 
      Stop(); //initialize with motors stoped
      //Change pin mode only if new command is different from previous.   
      //Serial.println(command);
      switch(command){
      case 'F':  
        forward();
        break;
      case 'B':  
        back();
        break;
      case 'L':  
        left();
        break;
      case 'R':
        right();
        break;
      case 'S':
        Stop();
        break;
      default:
      Stop();
      }     
    }

    if(lightSensorReading==HIGH)               //HIGH means,light got blocked
      digitalWrite(ledPin,HIGH);        //if light is not present,LED on
      else
      digitalWrite(ledPin,LOW);   
  }

}



void forward()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, carSpeed);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, carSpeed);
}

void back()
{
  digitalWrite(motor1pin1, HIGH);
  analogWrite(motor1pin2, carSpeed);

  digitalWrite(motor2pin1, HIGH);
  analogWrite(motor2pin2, carSpeed);
}

void left()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, carSpeed);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, 0);
}
void fastLeft()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, fastCarSpeed);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, 0);
}

void right()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, 0);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, carSpeed);
}
void fastRight()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, 0);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, fastCarSpeed);
}
void Stop()
{
  digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, 0);

  digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, 0);
}