#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int buttonPin = 2;

bool lastState = HIGH;
unsigned long pressStart = 0;
bool sent = false;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHAR_UUID    "abcd1234-5678-1234-5678-abcdef123456"

class Callbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    BLEDevice::getAdvertising()->start();
  }
};

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  BLEDevice::init("Ring-BT");

  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new Callbacks());

  BLEService *service = server->createService(SERVICE_UUID);

  pCharacteristic = service->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());

  service->start();
  BLEDevice::getAdvertising()->start();
}

void loop() {
  bool current = digitalRead(buttonPin);

  // 🔴 inizio pressione
  if (lastState == HIGH && current == LOW) {
    pressStart = millis();
    sent = false;
  }

  // ⏱️ dopo 2 secondi invia UNA volta
  if (current == LOW && !sent && millis() - pressStart >= 2000) {
    if (deviceConnected) {
      pCharacteristic->setValue("ale23");
      pCharacteristic->notify();
    }
    sent = true;
  }

  lastState = current;
}