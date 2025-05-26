#include "BNOWrapper.h"

namespace BNO{

static Adafruit_BNO08x  bno08x(-1);
static sh2_SensorValue_t sensorValue;

bool setupSensor(){
    Serial1.begin(921600, SERIAL_8N1, D4, D5); // Initialize UART at 115200 baud, RX=GPIO0, TX=GPIO1

    if (!bno08x.begin_UART(&Serial1, 0)) {
        Serial.println("Failed to initialize BNO085 sensor with UART");
        return false;
    }
    delay(50);
    Serial.println("BNO085 sensor initialized successfully via UART");
    
    Serial.println("Setting desired reports ");
    if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Could not enable game vector");
        return false;
    }
    return true;
}

bool getRot(quaternion::Quaternion& quat){
    if (! bno08x.getSensorEvent(&sensorValue)) {
        Serial.println("Problem with data");
        return false;
    }
    quaternion::Quaternion rot(
        sensorValue.un.gameRotationVector.real, 
        sensorValue.un.gameRotationVector.i,
        sensorValue.un.gameRotationVector.j,
        sensorValue.un.gameRotationVector.k);
    quat = rot;
    
    return true;
}

}


