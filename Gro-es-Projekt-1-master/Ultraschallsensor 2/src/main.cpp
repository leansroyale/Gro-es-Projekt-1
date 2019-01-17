#include <Arduino.h>
#include "LedControl.h"
#include "binary.h"
#include <AddicoreRFID.h>
#include <SPI.h>

#define uchar unsigned char
#define uint  unsigned int
#define trigPin A1
#define echoPin A0
#define RED 2
#define GREEN 7
#define MAX_LEN 16

uchar fifobytes;
uchar fifoValue;

AddicoreRFID myRFID;

const int chipSelectPin = 10;
const int NRSTPD = 5;

const int BUZZER = 3;

LedControl lc=LedControl(A3,A4,A5,1);
unsigned long delaytime=1000;
byte x[8]= {B10000001,B01000010,B00100100,B00011000,B00011000,B00100100,B01000010,B10000001,};





void setup()
{

  SPI.begin();
  pinMode(RED, OUTPUT);
  pinMode(chipSelectPin,OUTPUT);                                                // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
    digitalWrite(chipSelectPin, LOW);                                           // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                                                       // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

  pinMode(trigPin, OUTPUT);                                                     // trig pin will have pulses output
  pinMode(echoPin, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);                                                      // echo pin should be input to get pulse width

  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  myRFID.AddicoreRFID_Init();

  Serial.begin(9600);
}

void drawFaces(){
  // Display sad face
  lc.setRow(0,0,x[0]);
  lc.setRow(0,1,x[1]);
  lc.setRow(0,2,x[2]);
  lc.setRow(0,3,x[3]);
  lc.setRow(0,4,x[4]);
  lc.setRow(0,5,x[5]);
  lc.setRow(0,6,x[6]);
  lc.setRow(0,7,x[7]);
  delay(delaytime);

}

void loop()
{
                                                                                // Duration will be the input pulse width and distance will be the distance to the obstacle in centimeters
  int duration, distance;

  uchar i, tmp, checksum1;
  uchar status;
      uchar str[MAX_LEN];
      uchar RC_size;
      uchar blockAddr;  //Selection operation block address 0 to 63
      String mynum = "";



  digitalWrite(trigPin, HIGH);                                                  // Output pulse with 1ms width on trigPin
  delay (1);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);                                            // Measure the pulse input in echo pin

  distance = (duration/2) / 29.1;                                               // Distance is half the duration devided by 29.1 (from datasheet)

    if (distance <= 25 && distance >= 0) {                                      // if distance less than 0.5 meter and more than 0 (0 or less means over range)

      Serial.println("Object Detected");
      digitalWrite(RED, HIGH);
      tone(BUZZER, 1000);
      drawFaces();
    }




    while(str[0] == 166 && str[1] == 179 && str[2] == 242 && str[3] == 31)
    {

      str[1] = 0x4400;
//Find tags, return tag type
status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
if (status == MI_OK)
{
        Serial.println("\n \n \n");
        Serial.println("RFID ta³g detected");


}

//Anti-collision, return tag serial number 4 bytes
status = myRFID.AddicoreRFID_Anticoll(str);
if (status == MI_OK)
{

        checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
        Serial.print("The tag's number is:\t");
        Serial.print(str[0]);
          Serial.print(" , ");
        Serial.print(str[1]);
          Serial.print(" , ");
        Serial.print(str[2]);
          Serial.print(" , ");
        Serial.println(str[3]);
      }
    }




      Serial.println("RFID DETECTED");
      Serial.println("ALARM DEACTIVATED");
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, HIGH);
        delay(3000);
      noTone(BUZZER);
      lc.clearDisplay(0);



  {                                                                           // Waiting 60 ms won't hurt any one
    delay(1);
  }

myRFID.AddicoreRFID_Halt();

}
