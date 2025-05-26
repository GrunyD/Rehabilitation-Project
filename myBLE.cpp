#include "myBLE.h"

namespace myBLE{
/////////////////////////////////////////////////////
///////////////////     SERVER      /////////////////
/////////////////////////////////////////////////////
BLEServerHandler globalServer;

class CharCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override {
        onWriteVector(pCharacteristic);
    }

    void onWriteVector(BLECharacteristic *pCharacteristic) {
        String rxValue = String(pCharacteristic->getValue());
        if (rxValue.length() > 0) {
            Serial.print(rxValue.c_str()[0]);
            
            float x;
            float y;
            float z;
            int size = sizeof(float);
            memcpy(&x, rxValue.c_str() + 1, size);
            memcpy(&y, rxValue.c_str() + 1 + size, size);
            memcpy(&z, rxValue.c_str() + 1 + 2*size, size);

            Serial.print(" ");
            Serial.print(x, 6);
            Serial.print(" ");
            Serial.print(y, 6);
            Serial.print(" ");
            Serial.print(z, 6);
            Serial.println("");
        }
    }

    void onWriteQuaternion(BLECharacteristic *pCharacteristic) {
      String rxValue = String(pCharacteristic->getValue());
      if (rxValue.length() > 0) {
        Serial.print("Data ");
        Serial.print(rxValue.c_str()[0]);
        float w;
        float x;
        float y;
        float z;
        int size = sizeof(float);
        memcpy(&w, rxValue.c_str() + 1, size);
        memcpy(&x, rxValue.c_str() + 1 + 1*size, size);
        memcpy(&y, rxValue.c_str() + 1 + 2*size, size);
        memcpy(&z, rxValue.c_str() + 1 + 3*size, size);

        Serial.print("-");
        Serial.print("w: ");
        Serial.print(w);
        Serial.print(" x: ");
        Serial.print(x);
        Serial.print(" y: ");
        Serial.print(y);
        Serial.print(" z: ");
        Serial.print(z);
        Serial.println("");
      }
    }
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
        NimBLEDevice::startAdvertising();
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Client disconnected - start advertising\n");
        NimBLEDevice::startAdvertising();
    }

};
    
BLEServerHandler::BLEServerHandler() : pCharacteristic(nullptr){}//, writeCallback(nullptr) {}

void BLEServerHandler::start(){

    NimBLEDevice::init("RehabServer");
    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks);
    NimBLEService* pService = pServer->createService(serviceUUID);

    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE |
        NIMBLE_PROPERTY::NOTIFY
    );

    // pCharacteristic->addDescriptor(new NimBLE2902());
    pCharacteristic->setValue("Hello");
    pCharacteristic->setCallbacks(new CharCallbacks);
    pCharacteristic->addDescriptor(new NimBLE2904());

    pService->start();
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(serviceUUID);
    pAdvertising->setName("RehabServer");
    // pAdvertising->enableScanResponse(true);
    pAdvertising->start();
    // NimBLEDevice::getAdvertising()->start();
}

void BLEServerHandler::notify(const char* data) {
    if (pCharacteristic) {
        pCharacteristic->setValue(data);
        pCharacteristic->notify();
    }
}




/////////////////////////////////////////////////////
///////////////////     CLIENT      /////////////////
/////////////////////////////////////////////////////

BLEClientHandler globalClient;

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
     * Called for each advertising BLE server.
     */
     void onResult(BLEAdvertisedDevice advertisedDevice) {
       if (advertisedDevice.getName().length()>0){
         Serial.print("Found BLE device: ");
         Serial.println(advertisedDevice.toString().c_str());
         Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
         Serial.println(advertisedDevice.isAdvertisingService(serviceUUID));
       }
       
   
   
       if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
         Serial.print("Found our server! Address: ");
         BLEAddress* pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // Store the server address
         Serial.println(pServerAddress->toString().c_str());
        //  doConnect = true;
        //  doScan = false;
       } // Found our server
     } // onAdvertised
   }; // MyAdvertisedDeviceCallbacks

class MyClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pclient) override {
      Serial.println("Connected to the server!");
    }
  
    void onDisconnect(NimBLEClient* pClient, int reason) override {
        Serial.println("Disconnected from the server!");
      // It will put the chip into deep sleep if it fails to connect
    //   globalClient.connectToServer();
        esp_restart();
    }
  };


// BLEClientHandler* BLEClientHandler::activeInstance = nullptr;

BLEClientHandler::BLEClientHandler()
    : client(nullptr), characteristic(nullptr){}//, notifyCallback(nullptr) {}

bool BLEClientHandler::connectToServer() {
    NimBLEDevice::init("RehabClient");
    Serial.println("Connecting");
    delay(500);
    NimBLEScan* pscan = NimBLEDevice::getScan();
    // scan->setAdvertisedDeviceCallbacks(new AdvertiskedDeviceCallbacks());
    pscan->setActiveScan(true);
    NimBLEScanResults results = pscan->getResults(1000);
    for (int trial = 0; trial < 3; trial++){
        Serial.print("Trial ");
        Serial.println(trial + 1);
        for (int i = 0; i < results.getCount(); i++) {
            const NimBLEAdvertisedDevice* device = results.getDevice(i);
            if (device != nullptr){
                if (device->isAdvertisingService(serviceUUID)){
                    client = NimBLEDevice::createClient();
                    client->setClientCallbacks(new MyClientCallbacks());
                    if (client->connect(device)) {
                        NimBLERemoteService* pservice = client->getService(serviceUUID);
                        if (pservice != nullptr) {
                            delay(1000);
                            characteristic = pservice->getCharacteristic(characteristicUUID);
                            Serial.println("Got characteristics");
                            if (characteristic != nullptr) {
                                if (characteristic->canNotify()) {
                                    characteristic->subscribe(true, notifyCallback);
                                }
                                return true;
                            }
                        }
                        client->disconnect();
                    }
                }
            }
        }
        //Wait 3 seconds between trials and the LED is blinking
        for (int i = 0; i < 20; i++){
            if (i%2 == 0){
                digitalWrite(D1, HIGH);
            }
            else{
                digitalWrite(D1, LOW);
            }
            delay(100);
        }
    }
    Serial.println("Going to sleep now");
    digitalWrite(VCC_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    pinMode(LED_PIN, INPUT);
    delay(200);
    esp_deep_sleep_start();
    delay(200);
    return false;
}

String BLEClientHandler::read() {
    if (characteristic && characteristic->canRead()) {
        return characteristic->readValue().c_str();
    }
    return "";
}

void BLEClientHandler::write(const char* data) {
    if (characteristic && characteristic->canWrite()) {
        characteristic->writeValue(data);
    }
}

void BLEClientHandler::write_vector(float x, float y, float z){
    int size = sizeof(float);
    byte message[3*size + 1];
    last_x = x;
    last_y = y;
    message[0] = 'F';
    memcpy(message + 1, &x, size);
    memcpy(message + 1 + size, &y, size);
    memcpy(message + 1 + 2*size, &z, size);
    characteristic->writeValue(message, 3*size + 1);
}

void BLEClientHandler::write_quaternion(quaternion::Quaternion quaternion){
    int size = sizeof(float);
    byte message[3*size + 1];

    message[0] = 'F';
    memcpy(message + 1, &quaternion.w, size);
    memcpy(message + 1 + size, &quaternion.x, size);
    memcpy(message + 1 + 2*size, &quaternion.y, size);
    memcpy(message + 1 + 3*size, &quaternion.z, size);
    characteristic->writeValue(message, 4*size + 1);
}
float BLEClientHandler::last_x = 0.;
float BLEClientHandler::last_y = 1.;
// float BLEClientHandler::last_z = 0.;

void BLEClientHandler::notifyCallback(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
    Serial.println(data[0]);
    switch (data[0])
    {
    case PUNISH_COMMAND:
        haptic::punisher.trigger();
        break;

    case REWARD_COMMAND:
        haptic::rewarder.trigger();
        break;

    case CALIBRATE_COMMAND:{
        quaternion::Quaternion temp_rot;
        while (!BNO::getRot(temp_rot)){
          delay(10);
        }
        quaternion::ref_yaw = temp_rot.get_yaw();
        break;
    }
    default:
        break;
    }
}


}