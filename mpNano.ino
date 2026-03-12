#include <Adafruit_PN532.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h> 
#include <utility/imumaths.h>


#define PN532_IRQ (2)
#define PN532_RESET (3)


#define CARDREADREQUEST 201
#define NOTHINGDETECTED 202

#define HALFSTEPZEROPOINT 512
#define HALFSTEPLOWPOINT 630
#define HALFSTEPHIGHPOINT 365

#define OCTAVEZEROPOINT 489
#define OCTAVELOWPOINT 600
#define OCTAVEHIGHPOINT 340

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// HALFSTEPZEROPOINT, HALFSTEPLOWPOINT, HALFSTEPHIGHPOINT, OCTAVEZEROPOINT, OCTAVELOWPOINT, OCTAVEHIGHPOINT
uint16_t calibrationConstants[6] = { 512, 630, 365, 489, 600, 340 };

uint16_t sensorReading[8];
double sensorReadingFloat[8];
byte sensorReadingByte[8];

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

unsigned long lastNfcCheckTime = 0;
int nfcCheckInterval = 1500;
int response = 0;
int lastResponse = 0;
int tagID = 0;

bool externalMode = false;
bool lastExternalMode = false;

float pitch = 0.0;
float roll = 0.0;
float yaw = 0.0;

float angle = 0.0;
float magnitude = 0.0;

int currentQuadrant = 0;



void setup() {

  Serial.begin(115200);

  while (!nfc.begin()) {
    delay(10);
  }
}

void loop() {

  if (externalMode == true) {
    if (lastExternalMode == false) {
      while (!bno.begin()) {
        delay(10);
      }
      lastExternalMode = true;
    }

    sensors_event_t event;
    bno.getEvent(&event);

    pitch = event.orientation.z;
    yaw = event.orientation.x;
    roll = event.orientation.y;

    magnitude = sqrt(sq(roll) + sq(pitch));

    if (pitch > 0.0 && roll > 0.0) {
      currentQuadrant = 0;
    } else if (pitch > 0.0 && roll <= 0.0) {
      currentQuadrant = 1;
    } else if (pitch <= 0.0 && roll <= 0.0) {
      currentQuadrant = 2;
    } else if (pitch <= 0.0 && roll > 0.0) {
      currentQuadrant = 3;
    }

    if (currentQuadrant == 1 || currentQuadrant == 3) {
      angle = (acos(abs(pitch) / magnitude)) * 57.23 + (90 * currentQuadrant);
    } else {
      angle = (asin(abs(pitch) / magnitude)) * 57.23 + (90 * currentQuadrant);
    }

    for (int i = 0; i < 8; i++) {
      sensorReadingFloat[i] = cos(0.0174533 * (angle - (i * 45)));      // cosine calculation with radian conversion
      sensorReadingFloat[i] = pow(sensorReadingFloat[i], 11);           // exponent to drive down values of distant zones
      sensorReadingFloat[i] = (sensorReadingFloat[i] * magnitude) * 5;  // "5" is an arbitrary constant to increase overall output
      if (sensorReadingFloat[i] < 15.0) {
        sensorReadingFloat[i] = 0.0;  // nullify low values
      }
      sensorReading[i] = sensorReadingFloat[i];  // convert to int
      sensorReading[i] = constrain(sensorReading[i], 0, 254);
      sensorReadingByte[i] = sensorReading[i];
    }


    
    Serial.write(203);  // start byte for Sensor Readings

    while (!Serial.available()) {  // wait for acknowledgement
    }

    int ack = Serial.read();

    if (ack == 2) {
      for (int i = 0; i < 8; i++) {
        Serial.write(sensorReadingByte[i]);
        delayMicroseconds(600);
      }

      while (!Serial.available()) {  // wait for acknowledgement
      }

      int rec = Serial.read();
    }
  }


  
  if (externalMode == false) {
    if (millis() > lastNfcCheckTime + nfcCheckInterval) {  // if more than 1.5 seconds have elapsed
      lastNfcCheckTime = millis();

      Serial.write(CARDREADREQUEST);  // send request

      while (!Serial.available()) {  // wait for response
      }

      response = Serial.read();

      if (response == 0) {  // request denied
      }

      else if (response == 1) {  // request approved, check nfc, reinitialize nfc, send results
        checkNFC();
        nfc.begin();
        Serial.write(tagID);
      }
    }
  }
}



void checkNFC() {

  lastExternalMode = externalMode;

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

  if (success) {

    if (uidLength == 4) {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success) {
        uint8_t data[16];
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success) {
          tagID = data[0];

          if (tagID == 103) {
            externalMode = !externalMode;
          }
        }
      }
    }
  } else {
    tagID = NOTHINGDETECTED;
  }
}
