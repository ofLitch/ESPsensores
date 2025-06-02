// Librerías
#include <ThreeWire.h> 
#include <RtcDS1302.h>

#include "sensorData.h"
#include "sensor_MQ135.h"
#include "sensor_DHT.h"
#include "sensor_LDR.h"

// Definiciones
#define MQ135_AO_PIN 35                           ///< Pin analógico para MQ135
#define MQ135_VOLTAGE 4.0f                        ///< Voltaje en el que se alimenta el sensor MQ135 
#define MQ135_ADC_RESOLUTION 12                   ///< Resolución del ADC (12 bits para ESP32)
#define DHT_PIN   32                              ///< Pin para el sensor DHT
#define DHT_TYPE  DHT11                           ///< Tipo de sensor DHT usado
#define LDR_PIN 34                                ///< Pin analógico para el sensor LDR

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0; ///< CPU en el que se ejecutará la tarea (0 para un solo núcleo)
#else
  static const BaseType_t app_cpu = 1; ///< CPU en el que se ejecutará la tarea (1 para múltiples núcleos)
#endif

void setup() {
  // Definiciones de pines
  const unsigned short int mqPin = MQ135_AO_PIN;
  const unsigned short int dhtPin = DHT_PIN;
  const uint8_t dhtType = DHT_TYPE;
  const unsigned short int ldrPin = LDR_PIN;

  // Instancias
  SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
  static DHT dht(dhtPin, dhtType);
  static MQUnifiedsensor mqSensor("ESP-32", MQ135_VOLTAGE, MQ135_ADC_RESOLUTION, MQ135_AO_PIN, "MQ-135");

  // Inicializar
  Serial.begin(115200);
  dht.begin();

  // Parámetros para las tareas
  void *paramData[1] = {mutex}; 
  void *paramsDHT[3] = {mutex, &dht};
  void *paramsMQ135[2] = {mutex, &mqSensor};
  void *paramsLDR[2] = {mutex, (void *)(uintptr_t)ldrPin};

  // Tasks
  xTaskCreate(taskTemperature, "Read Temperature Function", 4096, paramsDHT, 2, NULL);
  xTaskCreate(taskHumidity, "Read Humidity Function", 4096, paramsDHT, 2, NULL);
  xTaskCreate(taskMQ135, "MQ135 task", 2048, paramsMQ135, 2, NULL);
  xTaskCreate(taskLDR, "LDR task", 2048, paramsLDR, 2, NULL);
}

void loop() {}