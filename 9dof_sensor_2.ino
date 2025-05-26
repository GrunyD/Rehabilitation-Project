#include <Adafruit_BNO08x.h>
#include <HardwareSerial.h>

#include "quaternion.h"
#include "myBLE.h"
#include "BNOWrapper.h"
#include "haptic.h"
#include "pins.h"


quaternion::Quaternion rot;

float temp_x, temp_y, temp_z;


void setup(void) {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    pinMode(VCC_PIN, OUTPUT);
    digitalWrite(VCC_PIN, HIGH);
    BNO::setupSensor();
 
    
    
    // Enable wake up from deep sleep by pressing button connecting pin D0 and GND
    esp_deep_sleep_enable_gpio_wakeup(BIT(WAKE_BUTTON_PIN), ESP_GPIO_WAKEUP_GPIO_LOW);

    // If it doesn't connect in 30 seconds, goes into deep sleep
    delay(2000);
    myBLE::globalClient.connectToServer();
    digitalWrite(LED_PIN, HIGH);
}

void loop(void){
    
    while (!BNO::getRot(rot)){
      delay(10);
    }


    temp_x = -1.;//cos(quaternion::ref_yaw);
    temp_y = 0.;//sin(quaternion::ref_yaw);
    temp_z = 0.;
    rot.rotateVector(temp_x, temp_y, temp_z);

    Serial.print("Vector: ");
    Serial.print(temp_x);
    Serial.print("  ");
    Serial.print(temp_y);
    Serial.print("  ");
    Serial.println(temp_z);
    float yaw = quaternion::ref_yaw;
    // yaw = 180*yaw/3.14159;
    float x = cos(-yaw)*temp_x - sin(-yaw)*temp_y;
    float y = sin(-yaw)*temp_x + cos(-yaw)*temp_y;
    myBLE::globalClient.write_vector(-x, y, temp_z);

    haptic::punisher.update();
    haptic::rewarder.update();
    delay(10);
}



////////////////////////////////////////
////////////////////////////////////////
// Working code for server !!!

// #include "myBLE.h"
// #include "haptic.h"

// void setup(){
//     Serial.begin(115200);
//     while (!Serial) delay(10);

//     myBLE::globalServer.start();
// }

// void loop(){
//     if (Serial.available()){
//         char inChar = (char)Serial.read();
//         if (inChar == PUNISH_COMMAND || inChar == REWARD_COMMAND || inChar == CALIBRATE_COMMAND){
//           Serial.print("User input: ");
//           Serial.println(inChar);
//           myBLE::globalServer.notify(&inChar);
//         }
//     }

// }

