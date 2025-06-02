// Librerías
#include <ThreeWire.h> 
#include <RtcDS1302.h>

#include "sensorData.h"
#include "sensor_MQ135.h"
#include "sensor_DHT.h"

// Definiciones para el sensor MQ135
#define MQ135_AO_PIN 35                   ///< Pin analógico para MQ135

// Definiciones para el sensor DHT
#define DHT_PIN   34                 ///< Pin para el sensor DHT
#define DHT_TYPE  11             ///< Tipo de sensor DHT usado

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0; ///< CPU en el que se ejecutará la tarea (0 para un solo núcleo)
#else
  static const BaseType_t app_cpu = 1; ///< CPU en el que se ejecutará la tarea (1 para múltiples núcleos)
#endif

void setup() {
  // Definiciones de pines
  const unsigned short int mqPin = MQ135_AO_PIN;
  const unsigned short int dhtPin = DHT_PIN;
  const unsigned short int dhtType = DHT_TYPE;  

  // Instancias
  SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

  // Inicializar
  Serial.begin(115200);

  // Parámetros para las tareas
  void *paramsMQ135[2] = {mutex, (void*)&mqPin};
  void *paramsDHT[3] = {mutex, (void*)&dhtPin, (void*)&dhtType};
  void *paramData[2] = {mutex};

  // Tasks
  xTaskCreate(taskMQ135, "MQ135 task", 2048, paramsMQ135, 2, NULL);
  //xTaskCreate(taskHumidity, "Read Humidity Function", 2048, paramsDHT, 2, NULL);
  //xTaskCreate(taskTemperature, "Read Temperature Function", 2048, paramsDHT, 2, NULL);
}

void loop() {}