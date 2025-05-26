#ifndef START_SENSOR_H
#define START_SENSOR_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Adafruit_BNO08x.h>
#include "quaternion.h"
#include "haptic.h"

namespace BNO{
    bool setupSensor();
    bool getRot(quaternion::Quaternion& quat);
}

#endif