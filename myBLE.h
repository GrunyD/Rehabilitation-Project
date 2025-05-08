#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>
#include <BLEDescriptor.h>
#include "commands.h"
#include <Arduino.h>


static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID characteristicUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
// const String serverName = 'GrunyRehabServer';


class BLEServerHandler {
    public:
        typedef void (*WriteCallback)(const char*);
    
        BLEServerHandler();

        void start();

        void notify(const char* data);
    
    private:
        BLECharacteristic* pCharacteristic;
        // WriteCallback writeCallback;
    
        class InternalCallbacks;
    };


class BLEClientHandler {
    public:
        typedef void (*NotifyCallback)(const char*);
    
        BLEClientHandler();

        bool connectToServer();
        String read();
        void write(const char* data);
        void write_vector(float x, float y, float z);
    
    private:
        BLEClient* client;
        BLERemoteCharacteristic* characteristic;
    
        // static BLEClientHandler* activeInstance;
        static void notifyCallback(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify);
    };

extern BLEClientHandler globalClient;

#endif