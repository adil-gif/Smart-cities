#include "tasks.hh"
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <esp32/ulp.h>
#include <esp_task_wdt.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP280.h>

#define WDT_TIMEOUT 3
#define THRESHOLD 80 

typedef const int Capacity;
String data;
String comp;
typedef struct Sensor
{
  float Temperature;
  uint8_t Humidity;
  float Pressure;
  float Luminosity;
  float NoiseLevel;
  float CO2;
};

Tasks tasks(data, comp);
Capacity capacity = 1*JSON_OBJECT_SIZE(3)+3*JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> doc;

Sensor sensor1;
Sensor sensor2;
Sensor sensor3;
Sensor sensor4;

void callback(){

}
/////////////////////////////// Tasks function //////////////////////////////////////////////
void sensor1Task(void *pvParameters)
{  
  const char *pcTaskName = "sensor1";
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  UBaseType_t uxPriority;
  sensor1.Temperature = bmp1.readTemperature();
  uxPriority = uxTaskPriorityGet( NULL );

  for(;;)
  {
    Serial.printf("%s - envoyé = %f (priorite %d)\n", pcTaskName, sensor1.Temperature, uxPriority);
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 4000 ) );
  }
}

//////////////////////////////////// task of sensor 2 //////////////////////////////////////
void sensor2Task(void *pvParameters)
{
  const char *pcTaskName = "sensor2";
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  sensor2.Luminosity = 60;
  UBaseType_t uxPriority;
  uxPriority = uxTaskPriorityGet( NULL );

  for(;;)
  {
      Serial.printf("%s - envoyé = %f (priorite %d)\n", pcTaskName, sensor2.Luminosity, uxPriority);
      vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 4000 ) );
  }
}

//////////////////////////////////// task of sensor 3 //////////////////////////////////////
void sensor3Task(void *pvParameters)
{
  const char *pcTaskName = "sensor3";
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  sensor3.NoiseLevel = 55;
  UBaseType_t uxPriority;
  uxPriority = uxTaskPriorityGet( NULL );

  for(;;)
  {
    Serial.printf("%s - envoyé = %f (priorite %d)\n", pcTaskName, sensor3.NoiseLevel, uxPriority);
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 4000 ) );
  }
}

//////////////////////////////////// task of sensor 4 //////////////////////////////////////
void sensor4Task(void *pvParameters)
{
  const char *pcTaskName = "sensor4";
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  sensor4.CO2 = 200;
  UBaseType_t uxPriority;
  uxPriority = uxTaskPriorityGet( NULL );
  for(;;)
  {
    Serial.printf("%s - envoyé = %f (priorite %d)\n", pcTaskName, sensor4.CO2, uxPriority);
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 4000 ) );
  }
}

//////////////////////////////////// task of lora sender //////////////////////////////////////
void loraSenderTask(void *pvParameters)
{
  const int SCK = 5;
  const int MISO = 19;
  const int MOSI = 27;
  const int SS = 18;
  const int RST = 14;
  const int DIO0 = 26;
  const char *pcTaskName = "loraSenderTask";
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  UBaseType_t uxPriority;
  uxPriority = uxTaskPriorityGet( NULL );
  //int counter = 0;

  //Serial.println("LoRa Sender");
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(8681E5)) {
    //Serial.println("Starting LoRa failed!");
    while (1);
  }
  for(;;)
  {
    // Serial.printf("%s - lu = %d (priorite %d)\n", pcTaskName, receivedValueFromSensors, uxPriority);
    Serial.println("Sending packet: ");
    LoRa.beginPacket();
    LoRa.println(comp);
    Serial.println(comp);
    Serial.println("sending data is done ");
    LoRa.endPacket();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 6000 ) );
  }
  // vTaskDelay(pdMS_TO_TICKS(100));
}

//////////////////////////////////// task of lora receiver //////////////////////////////////////
void loraReceiverTask(void *pvParameters)
{
  const int SCK = 5;
  const int MISO = 19;
  const int MOSI = 27;
  const int SS = 18;
  const int RST = 14;
  const int DIO0 = 26;
  for(;;)
  {
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);
    if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      //received a packet
      Serial.print("Received packet ");

      //read packet
      while (LoRa.available()) {
      data = LoRa.readString();
      Serial.print(data);
      }
    }
  }
}

//////////////////////////////////// task of deep sleep //////////////////////////////////////
void sleepModeTask(void *pvParameters)
{
  const char *pcTaskName = "sleepModeTask";
  // TickType_t xLastWakeTime;
  // xLastWakeTime = xTaskGetTickCount();
  touchAttachInterrupt(T3, callback, THRESHOLD);
  // touch_pad_t esp_sleep_get_touchpad_wakeup_status();
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
  for (;;)
  { 
    Serial.println("sleep mode");
    // esp_sleep_enable_timer_wakeup(100);
    Serial.printf("%s -sleep mode", pcTaskName);
    // vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 100 ) );
  }
}

//////////////////////////////////// task of watch dog //////////////////////////////////////
void watchDogTask(void *pvParameters)
{
    UBaseType_t uxPriority;
    uxPriority = uxTaskPriorityGet( NULL );
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    // Serial.println("Configuring WDT...");
    esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
    esp_task_wdt_add(NULL); //add current thread to WDT watch

    int i = 0;
    int last = millis();
  // resetting WDT every 2s, 5 times only
  for(;;)
  {
    if (millis() - last <= 4000) {
        //Serial.println("Resetting WDT...");
        esp_task_wdt_reset();
        last = millis();
        // if (i == 5) {
        //     Serial.println("Stopping WDT reset. CPU should reboot in 3s");
        // }
      }
  }
  vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 4000 ) );
}

void JsonTask(void *pvParameters){
    String output = "";
    const char *pcTaskName = "JsonTask";
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
  for(;;){
    JsonObject sensorObj1 = doc.createNestedObject("Sensor1");
    sensorObj1["Temperature"] = sensor1.Temperature;
    sensorObj1["Humidity"] = sensor1.Temperature;
    sensorObj1["Pressure"] = sensor1.Temperature;
    JsonObject sensorObj2 = doc.createNestedObject("Sensor2");
    sensorObj2["luminosity"]= sensor2.Luminosity;
    JsonObject sensorObj3 = doc.createNestedObject("Sensor3");
    sensorObj3["noise Level"] = sensor3.NoiseLevel;
    JsonObject sensorObj4 = doc.createNestedObject("Sensor4");
    sensorObj4["CO2 range"] = sensor4.CO2;
    serializeJson(doc, output);
    if( output == comp){
        doc.clear();
        output.clear();
    }else{
      Serial.println(output);
      comp = output;
    }
    
    doc.clear();
    output.clear();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 6000 ) );
  }
}

void Tasks :: tasksAppellation(char* Name[8], int StackSize[8],int Priority[8], int CoreID[8]){

        xTaskCreatePinnedToCore(sensor1Task, Name[0], StackSize[0], NULL, Priority[0], NULL, CoreID[0]);
        xTaskCreatePinnedToCore(sensor2Task, Name[1], StackSize[1], NULL, Priority[1], NULL, CoreID[1]);
        xTaskCreatePinnedToCore(sensor3Task, Name[2],StackSize[2], NULL, Priority[2], NULL, CoreID[2]);
        xTaskCreatePinnedToCore(sensor4Task, Name[3], StackSize[3], NULL, Priority[3], NULL, CoreID[3]);
        xTaskCreatePinnedToCore(loraSenderTask,  Name[4], StackSize[4], NULL, Priority[4], NULL, CoreID[4]);
        xTaskCreatePinnedToCore(loraReceiverTask,  Name[5], StackSize[5], NULL, Priority[5], NULL, CoreID[5]);
        xTaskCreatePinnedToCore(JsonTask, Name[6], StackSize[6], NULL, Priority[6], NULL, CoreID[6]);
        xTaskCreatePinnedToCore(watchDogTask, Name[7], StackSize[7], NULL, Priority[7], NULL, CoreID[7]);
}