#include <Arduino.h>
#include "ESP32_Sensor_Node.h"

// -----------------------------------------------------------------------------
// setup
// -----------------------------------------------------------------------------

void setup() {  client.disconnect();
  Serial.begin(9600);
  delay(100);
  
  //pinMode(lightpin, INPUT);
  //pinMode(motionpin, INPUT);    // initialize sensor as an input
  
  pinMode(GPIO_DONE, OUTPUT);
  digitalWrite(GPIO_DONE, LOW);
  #if LED_connected
  pinMode(LED_PIN, OUTPUT);    
  #endif
  
  #if BME_connected
  pinMode(BME_Power_Pin,OUTPUT);
  bme.begin(0x76);   
  #endif

  #if Battery_powered
  pinMode(batpin, INPUT);
  #endif

  #if HDC1080_connected
  hdc1080.begin(0x40);
  #endif

  readsensordata();
  printsensordata();

  setup_wifi();

  Serial.println("HTTP server started");

  Serial.println("\nThis is the Node 2!\n");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  #if LED_connected
  // this resets all the neopixels to an off state
  strip.begin();
  strip.clear();
  strip.show();
  #endif
  #if Doorsensor_connected
  pinMode(Doorsensorpin, INPUT_PULLUP);
  #endif
}

// -----------------------------------------------------------------------------
// loop 
// -----------------------------------------------------------------------------

void loop() {
 if (!client.connected()) {
    reconnect();
  }
  client.loop();
  #if BME_connected
  digitalWrite(BME_Power_Pin, HIGH);
  #endif
  delay(50);
  readsensordata();
  printsensordata();
  #if BME_connected
  digitalWrite(BME_Power_Pin, LOW);
  #endif
  #if Doorsensor_connected
  DoorSensor();
  #endif
  pushtopics();
  delay(10);
  digitalWrite(GPIO_DONE, HIGH);

#if SLEEP
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * S_TO_M_FACTOR * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
#endif
}
