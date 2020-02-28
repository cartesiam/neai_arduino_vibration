#include <NanoEdgeAI.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

//#define DATA_LOGGING

#define DATA_INPUT_USER 256
#define AXIS_NUMBER 3
#define NUMBER_LEARN 100
float acc_buffer[DATA_INPUT_USER * AXIS_NUMBER] = { 0 };

float acc_x = 0.F;
float acc_y = 0.F;
float acc_z = 0.F;
float last_acc_x = 0.F;
float last_acc_y = 0.F;
float last_acc_z = 0.F;

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!
  lis.setDataRate(LIS3DH_DATARATE_400_HZ);
  
  Serial.println("Range = "); Serial.println(2 << lis.getRange());  
  Serial.println("G");

#ifndef DATA_LOGGING
  NanoEdgeAI_initialize();
  for (int i = 0; i < NUMBER_LEARN; i++) {
    fill_acc_buffer();  
    NanoEdgeAI_learn(acc_buffer);
    Serial.println("Learning number ");
    Serial.print(i);
  }
#endif
}

void loop() {

  fill_acc_buffer();
#ifdef DATA_LOGGING
  for (int i = 0; i < DATA_INPUT_USER; i++) {
    Serial.print(acc_buffer[AXIS_NUMBER * i]);
    Serial.print(" ");
    Serial.print(acc_buffer[AXIS_NUMBER * i + 1]);
    Serial.print(" ");
    Serial.print(acc_buffer[AXIS_NUMBER * i + 2]);
    Serial.print(" ");
  }
  Serial.println(" ");

#else
  float score = NanoEdgeAI_detect(acc_buffer);
  Serial.println("score = ");
  Serial.print(score);
#endif

}

void fill_acc_buffer() {
    for (int i = 0; i < DATA_INPUT_USER; i++) {
      get_acc_values();
      acc_buffer[AXIS_NUMBER * i] = acc_x;
      acc_buffer[AXIS_NUMBER * i + 1] = acc_y;
      acc_buffer[AXIS_NUMBER * i + 2] = acc_z;
  }
}

void get_acc_values()
{
  /* Polling method to get a complete buffer */
  do {
    lis.read(); 
    acc_x = lis.x;
    acc_y = lis.y;
    acc_z = lis.z;
  }
  while (acc_x == last_acc_x && acc_y == last_acc_y && acc_z == last_acc_z);

  
  last_acc_x = acc_x;
  last_acc_y = acc_y;
  last_acc_z = acc_z;
}
