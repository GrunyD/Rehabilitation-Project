#include "myBLE.h"

/////////////////////////////////////////////////////
///////////////////     SERVER      /////////////////
/////////////////////////////////////////////////////

class CharCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = String(pCharacteristic->getValue());
      if (rxValue.length() > 0) {
        Serial.print(rxValue.c_str()[0]);
        Serial.print("- x: ");
        float x;
        float y;
        float z;
        int size = sizeof(float);
        memcpy(&x, rxValue.c_str() + 1, size);
        memcpy(&y, rxValue.c_str() + 1 + size, size);
        memcpy(&z, rxValue.c_str() + 1 + 2*size, size);

        Serial.print(x);
        Serial.print(", y: ");
        Serial.print(y);
        Serial.print(", z: ");
        Serial.print(z);
        Serial.println("");
      }
    }
};

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
        Serial.println("Client connected!");
        BLEClient* pClient = new BLEClient();
        //pClient->setConnId(param->connect.conn_id);
        // connectedClients.push_back(pClient);
        // pCharacteristic->setValue("Hello"); // Send initial message
        // pCharacteristic->notify();
    };

    void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
        Serial.println("Client disconnected!");
        // for (auto it = connectedClients.begin(); it != connectedClients.end(); ++it) {
        //     if ((*it)->getConnId() == param->connect.conn_id) {
        //         delete *it;
        //         connectedClients.erase(it);
        //         break;
        //     }
        // }
    }
};
    
BLEServerHandler::BLEServerHandler() : pCharacteristic(nullptr){}//, writeCallback(nullptr) {}

void BLEServerHandler::start(){

    BLEDevice::init("JouRehabServer");
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks);
    BLEService* pService = pServer->createService(serviceUUID);

    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new CharCallbacks);

    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(serviceUUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
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

class MyClientCallbacks : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
      Serial.println("Connected to the server!");
    }
  
    void onDisconnect(BLEClient* pclient) {
      Serial.println("Disconnected from the server!");
      // It will put the chip into deep sleep if it fails to connect
      globalClient.connectToServer();
    }
  };


// BLEClientHandler* BLEClientHandler::activeInstance = nullptr;

BLEClientHandler::BLEClientHandler()
    : client(nullptr), characteristic(nullptr){}//, notifyCallback(nullptr) {}

bool BLEClientHandler::connectToServer() {
    BLEDevice::init("MyESP32Client");
    BLEScan* scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    scan->setActiveScan(true);
    BLEScanResults* results = scan->start(5);

    for (int trial = 0; trial < 5; trial++){
        Serial.print("Trial ");
        Serial.println(trial);
        for (int i = 0; i < results->getCount(); i++) {
            BLEAdvertisedDevice device = results->getDevice(i);
            if (device.haveServiceUUID() && device.getServiceUUID().equals(serviceUUID)) {
                client = BLEDevice::createClient();
                client->setClientCallbacks(new MyClientCallbacks());
                if (client->connect(&device)) {
                    BLERemoteService* service = client->getService(serviceUUID);
                    if (service) {
                        characteristic = service->getCharacteristic(characteristicUUID);
                        if (characteristic) {
                            if (characteristic->canNotify()) {
                                // activeInstance = this;
                                characteristic->registerForNotify(notifyCallback);
                            }
                            return true;
                        }
                    }
                }
            }
        }
        delay(5000);
    }
    Serial.println("Going to sleep now");
    digitalWrite(D1, LOW);
    pinMode(D1, INPUT);
    delay(20);
    esp_deep_sleep_start();
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

    message[0] = 'A';
    memcpy(message + 1, &x, size);
    memcpy(message + 1 + size, &y, size);
    memcpy(message + 1 + 2*size, &z, size);
    characteristic->writeValue(message, 3*size + 1);
}

void BLEClientHandler::notifyCallback(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
    Serial.println(data[0]);
    switch (data[0])
    {
    case PUNISH_COMMAND:
        punisher.trigger();
        break;
    case REWARD_COMMAND:
        rewarder.trigger();
        break;
    
    default:
        break;
    }
    // Serial.println("Notification/Indication received: ");
    // if (activeInstance && activeInstance->notifyCallback) {
    //     char buffer[256];
    //     size_t copyLen = (length < 255) ? length : 255;
    //     memcpy(buffer, data, copyLen);
    //     buffer[copyLen] = '\0';
    //     activeInstance->notifyCallback(buffer);
    // }
}