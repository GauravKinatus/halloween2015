/*****************************
Mjolnir (Thor's hammer) project - 

This sketch will disengage an electromagnet when
a trusted tag is read with the 
ID-12/ID-20 RFID module

Pinout for SparkFun RFID USB Reader
Arduino ----- RFID module
5V            VCC
GND           GND
D4            TX

Pinout for SparkFun RFID Breakout Board
Arduino ----- RFID module
5V            VCC
GND           GND
D4            D0


Inspired by & partially adapted from
http://bildr.org/2011/02/rfid-arduino/
https://www.youtube.com/watch?v=0_8Xhzt5YQI

Modified by: Gaurav Garg
email: gaurav@kinatus.com
@gaurav_kinatus
******************************/

#include <SoftwareSerial.h>

// Choose two pins for SoftwareSerial
SoftwareSerial rSerial(4, 5); // RX, TX


// Pick a PWM pin to drive the electromagnet on a relay
const int relayPin = 10;

// For SparkFun's tags, we will receive 16 bytes on every
// tag read, but throw four away. The 13th space will always
// be 0, since proper strings in Arduino end with 0

// These constants hold the total tag length (tagLen) and
// the length of the part we want to keep (idLen),
// plus the total number of tags we want to check against (kTags)
const int tagLen = 16;
const int idLen = 13;
const int kTags = 2;

// Put your known tags here!
char knownTags[kTags][idLen] = {
             "7C0082EECBDB",
             "7C0082CFB485"
             
};

// Empty array to hold a freshly scanned tag
char newTag[idLen];

void setup() {
  // Starts the hardware and software serial ports
   Serial.begin(9600);
   rSerial.begin(9600);
  
  pinMode(relayPin, OUTPUT);
 

}

void loop() {
  // Counter for the newTag array
  int i = 0;
  // Variable to hold each byte read from the serial buffer
  int readByte;
  // Flag so we know when a tag is over
  boolean tag = false;

  //activate the relay from the begining
  digitalWrite(relayPin, HIGH);
  
  // This makes sure the whole tag is in the serial buffer before
  // reading, the Arduino can read faster than the ID module can deliver!
  if (rSerial.available() == tagLen) {
    tag = true;
  }

  if (tag == true) {
    while (rSerial.available()) {
      // Take each byte out of the serial buffer, one at a time
      readByte = rSerial.read();

      /* This will skip the first byte (2, STX, start of text) and the last three,
      ASCII 13, CR/carriage return, ASCII 10, LF/linefeed, and ASCII 3, ETX/end of 
      text, leaving only the unique part of the tag string. It puts the byte into
      the first space in the array, then steps ahead one spot */
      if (readByte != 2 && readByte!= 13 && readByte != 10 && readByte != 3) {
        newTag[i] = readByte;
        i++;
      }

      // If we see ASCII 3, ETX, the tag is over
      if (readByte == 3) {
        tag = false;
      }

    }
  }


  // don't do anything if the newTag array is full of zeroes
  if (strlen(newTag)== 0) {
    return;
  }

  else {
    int total = 0;
    int currentResult = 0;

    for (int ct=0; ct < kTags; ct++){
      currentResult = checkTag(newTag, knownTags[ct]);
      //Debug message to capture a new tag from the serial monitor
      Serial.print("Current Tag: "); Serial.print(newTag); Serial.print(" vs "); Serial.print(knownTags[ct]); Serial.print(" Result: "); Serial.println(total);
      total = total + currentResult;  
    }

    // If newTag matched any of the tags
    // we checked against, total will be greater than zero
    if (total > 0) {

      //activate the relay from the begining
      digitalWrite(relayPin, LOW);
    
      Serial.println("Success!");
      
      delay(10000);//give it some time and reset the relay
      //activate the relay and activate the magnet
      digitalWrite(relayPin, HIGH);
    }

    else {
        // This prints out unknown cards so you can add them to your knownTags as needed
        Serial.print("Unknown tag! ");
        Serial.print(newTag);
        Serial.println();
        
        digitalWrite(relayPin, HIGH);//re-activate the relay
    } 
  }

  // Once newTag has been checked, fill it with zeroes
  // to get ready for the next tag read
  for (int c=0; c < idLen; c++) {
    newTag[c] = 0;
  }
}

// This function steps through both newTag and one of the known
// tags. If there is a mismatch anywhere in the tag, it will return 0,
// but if every character in the tag is the same, it returns 1
int checkTag(char nTag[], char oTag[]) {
    for (int i = 0; i < idLen; i++) {
      if (nTag[i] != oTag[i]) {
        return 0;
      }
    }
  return 1;
}

