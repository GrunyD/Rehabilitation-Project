#include <Adafruit_BNO08x.h>
#include "quaternion.h"
#include "myBLE.h"
#include <HardwareSerial.h>
// #include "sensorWrappers.h"
#include "commands.h"


Quaternion rot;

Adafruit_BNO08x  bno08x(-1);
sh2_SensorValue_t sensorValue;

float ref_x = 0, ref_y = 1, ref_z = 0;
float temp_x, temp_y, temp_z;


void setup(void) {
    Serial.begin(115200);
    
    // while (!setupSensor()){
    //   Serial.println(("INit loop"));
    //     delay(200);
    // }
    Serial1.begin(115200, SERIAL_8N1, D4, D5); // RX = D4, TX = D5

    while (!bno08x.begin_UART(&Serial1, 0)) {
        Serial.println("Failed to initialize BNO085 sensor with UART");
        // return false;
        delay(1000);
    }
    delay(50);
    Serial.println("BNO085 sensor initialized successfully via UART");
    Serial.println("Setting desired reports ");
    if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Could not enable game vector");
        // return false;
    }
    // return true;

    // Turn on the LED to see it is on
    pinMode(D1, OUTPUT);
    digitalWrite(D1, HIGH);

    // Enable wake up from deep sleep by pressing button connecting pin D0 and GND
    esp_deep_sleep_enable_gpio_wakeup(BIT(D0), ESP_GPIO_WAKEUP_GPIO_LOW);

    // If it doesn't connect in 30 seconds, go into deep sleep
    globalClient.connectToServer();
    
}

void loop(void){
    
    while (!getRot(rot)){
      delay(10);
    }


    temp_x = ref_x;
    temp_y = ref_y;
    temp_z = ref_z;
    rot.rotateVector(temp_x, temp_y, temp_z);
    Serial.print("Vector: ");
    Serial.print(temp_x);
    Serial.print("  ");
    Serial.print(temp_y);
    Serial.print("  ");
    Serial.println(temp_z);

    globalClient.write_vector(temp_x, temp_y, temp_z);

    punisher.update();
    rewarder.update();
    delay(50);
}

bool getRot(Quaternion& quat){
    if (! bno08x.getSensorEvent(&sensorValue)) {
        Serial.println("Problem with data");
        return false;
    }
    Quaternion rot(
        sensorValue.un.gameRotationVector.real, 
        sensorValue.un.gameRotationVector.i,
        sensorValue.un.gameRotationVector.j,
        sensorValue.un.gameRotationVector.k);
    quat = rot;
    
    return true;
}



////////////////////////////////////////
////////////////////////////////////////
// Working code for server !!!

// #include "myBLE.h"
// #include "commands.h"
// BLEServerHandler server;

// void setup(){
//     Serial.begin(115200);
//     while (!Serial) delay(10);

    
//     server.start();
// }

// void loop(){
//     if (Serial.available()){
//         char inChar = (char)Serial.read();
//         if (inChar == PUNISH_COMMAND || inChar == REWARD_COMMAND || inChar == 'C'){
//           Serial.print("User input: ");
//           Serial.println(inChar);
//           server.notify(&inChar);
//         }
//     }

// }



// #include "DeepSleepHandler.h"

// #define BUTTON_PIN D0  // D0 on XIAO ESP32-C3

// // DeepSleepHandler sleepHandler(BUTTON_PIN);

// void setup() {
//     Serial.begin(115200);
//     // Allow serial monitor to connect
//     pinMode(D10, OUTPUT);
//     digitalWrite(D10, HIGH);
//     delay(500);
//     Serial.println("Wakey wakey");
//     // sleepHandler.begin();

//     // if (sleepHandler.wokeFromDeepSleep()) {
//     //     Serial.println("Woke from deep sleep!");
//     // } else {
//     //     Serial.println("Cold boot.");
//     // }
//     esp_deep_sleep_enable_gpio_wakeup(BIT(D0), ESP_GPIO_WAKEUP_GPIO_LOW);
//     delay(3000); 
// }

// void loop() {
//   if (digitalRead(D0) == LOW){
//     Serial.println("Going to sleep now");
//     esp_deep_sleep_start();
//     Serial.println("This will never be printed");
//   }
//   delay(20);
    // sleepHandler.checkSleepTrigger();

    // Do other stuff here
// }