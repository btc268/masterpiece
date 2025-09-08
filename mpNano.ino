#include <Adafruit_PN532.h>

#define PN532_IRQ (2)
#define PN532_RESET (3)


#define CARDREADREQUEST 201
#define NOTHINGDETECTED 202


Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

unsigned long lastNfcCheckTime = 0;
int nfcCheckInterval = 1500;
int response = 0;
int lastResponse = 0;
int tagID = 0;





void setup() {

  Serial.begin(115200);

  while (!nfc.begin()) {
    delay(10);
  }
}

void loop() {

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
      Serial.write(tagID)
    }
  }
}


void checkNFC() {

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
        }
      }
    }
  } else {
    tagID = NOTHINGDETECTED;  
  }
}
