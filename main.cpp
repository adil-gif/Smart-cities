#include <Arduino.h>
#include "tasks.hh"

char* Name[8]= {"sensor1", "sensor2", "sensor3", "sensor4", "LoRa Sender", "LoRa Receiver", "Json", "Watch Dog"};
int StackSize[8]= {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
int Priority[8] = {11, 10, 9, 8, 6, 5, 7, 4};
int CoreID[8] = {0, 0, 0, 0, 0, 0, 0, 1};
void setup() {
  //##### initiate serial monitor #####
  Serial.begin(115200);
  while (!Serial); 
  Serial.println("Start");
  String data = "", comp = "";
   bmp1.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500); 
  /////////////// tasks appellation /////////////////
  Tasks tasks(data, comp);
  tasks.tasksAppellation(Name, StackSize, Priority, CoreID);
}

void loop() {
  // put your main code here, to run repeatedly:
}