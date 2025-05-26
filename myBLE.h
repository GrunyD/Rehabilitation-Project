#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <NimBLEDevice.h>
#include <NimBLE2904.h>
#include <Arduino.h>

#include "BNOWrapper.h"
#include "haptic.h"
#include "quaternion.h"
#include "pins.h"


namespace myBLE{

static NimBLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static NimBLEUUID characteristicUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
// const String serverName = 'GrunyRehabServer';


class BLEServerHandler {
    public:
        typedef void (*WriteCallback)(const char*);
    
        BLEServerHandler();

        void start();

        void notify(const char* data);
    
    private:
        NimBLECharacteristic* pCharacteristic;
        // WriteCallback writeCallback;
    
        class InternalCallbacks;
    };


class BLEClientHandler {
    public:
        static float last_x, last_y;
        typedef void (*NotifyCallback)(const char*);
    
        BLEClientHandler();

        bool connectToServer();
        String read();
        void write(const char* data);
        void write_vector(float x, float y, float z);
        void write_quaternion(quaternion::Quaternion quaternion);
    
    private:
        // float last_x, last_y;
        NimBLEClient* client;
        NimBLERemoteCharacteristic* characteristic;
    
        // static BLEClientHandler* activeInstance;
        static void notifyCallback(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify);
    };

extern BLEServerHandler globalServer;
extern BLEClientHandler globalClient;

}

#endif