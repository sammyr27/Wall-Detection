#include "BLEDevice.h"

#define TRIG_PIN 32 // Trigger pin
#define ECHO_PIN 33 // Echo pin

#define bleServerName "CBLE"

static BLEUUID serviceUUID(
  "ce4fe722-2523-47fb-99c1-7f8c321dc378"
);

static BLEUUID distanceUUID(
  "55a1a343-d4e2-4f60-999a-a858103a09ad"
);

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* distanceCharacteristic;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {
      advertisedDevice.getScan()->stop(); // Stop advertising once ESP connects to PSOC
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
      Serial.println("PSoC found. Connecting...");
    }
  }
};

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send trigger signal
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure time for echo to return
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // If there is no time between trigger output and echo input, distance = 0
  if (duration == 0) {
    return 0.0;
  }

  return duration * 0.034 / 2;  // Convert distance to time
}

bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();

  pClient->connect(pAddress);
  Serial.println("Connected to PSoC");

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);

  if (pRemoteService == nullptr) {
    Serial.println("Failed to find service UUID");
    return false;
  }

  distanceCharacteristic = pRemoteService->getCharacteristic(distanceUUID);

  if (distanceCharacteristic == nullptr) {
    Serial.println("Failed to find distance characteristic UUID");
    return false;
  }

  Serial.println("Found distance characteristic");
  return true;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);

  Serial.println("ESP32 scanning for PSoC...");
}

void loop() {
  if (doConnect) {
    connected = connectToServer(*pServerAddress);
    doConnect = false;
  }

  if (connected && distanceCharacteristic != nullptr) {
    float distance = readDistanceCM();

    // Print out distance (cm) to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    distanceCharacteristic->writeValue(
      (uint8_t*)&distance,
      sizeof(distance),
      true
    );

  }

  delay(500); //500 ms delay for psoc to process
}