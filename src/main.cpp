// TODO: Hace falta
// Manejo de errores, manejo de excepciones
/**
 * @file main.cpp
 * @brief Programa principal para el monitoreo y control ambiental en un invernadero de cannabis medicinal.
 * 
 * @details Este programa utiliza RTOS para realizar el monitoreo y control ambiental dentro de un invernadero.
 *          Incluye lectura de sensores, control de actuadores, registro de datos, y soporte para alarmas visuales y sonoras.
 * 
 * @author
 *          Valentina Muñoz Arcos
 *          Luis Miguel Gómez Muñoz
 *          David Alejandro Ortega Flórez
 * 
 * @version 3.2
 * @date 2025-06-02
 */

/* =========== Librerías ============ */
#include <ThreeWire.h> 
#include <RtcDS1302.h>

#include "sensorData.h"
#include "sensor_MQ135.h"
#include "sensor_DHT.h"
#include "sensor_LDR.h"
#include "sensor_YL69.h"
#include "printData.h"
#include "esp_sensores_now.h"

/* =========== Definiciones ============ */
// Sensores
#define DHT_TYPE              DHT11                 ///< Tipo de sensor DHT usado
#define DHT_PIN               32                    ///< Pin para el sensor DHT
#define YL69_PIN              33                    ///< Pin analógico para el sensor YL69
#define LDR_PIN               34                    ///< Pin analógico para el sensor LDR
#define MQ135_AO_PIN          35                    ///< Pin analógico para MQ135
#define MQ135_VOLTAGE         4.0f                  ///< Voltaje en el que se alimenta el sensor MQ135
#define MQ135_ADC_RESOLUTION  12                    ///< Resolución del ADC (12 bits para ESP32)
// ESP-NOW
//#define BROADCAST_ADDRESS     {0xF4, 0x65, 0x0B, 0xE5, 0x49, 0x88} ///< Dirección MAC de broadcast para ESP-NOW F4:65:0B:E5:49:88

/* =========== Configuración FreeRTOS ============== */
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0; ///< CPU en el que se ejecutará la tarea (0 para un solo núcleo)
#else
  static const BaseType_t app_cpu = 1; ///< CPU en el que se ejecutará la tarea (1 para múltiples núcleos)
#endif


/**
 * @brief Configuración inicial del programa.
 * Esta función se ejecuta una vez al inicio del programa y configura los pines, inicializa los sensores y crea las tareas de FreeRTOS.
 * @note Se crean tareas para leer los sensores DHT, MQ135, YL69 y LDR, así como una tarea para imprimir los datos leídos.
 * @details
 * - Se definen los pines para cada sensor.
 * - Se crean instancias de los sensores DHT y MQ135.
 * - Se inicializan los sensores y se configuran los parámetros necesarios.
 * - Se crean las tareas de FreeRTOS para leer los sensores y para imprimir los datos.
 * @warning Asegúrate de que los pines definidos coincidan con tu hardware.
*/
void setup() {
  // Constantes de Pines y Parámetros
  const uint8_t dhtType = DHT_TYPE;
  const unsigned short int dhtPin = DHT_PIN;      // ( Pin 32 para DHT11 )
  const unsigned short int ylPin = YL69_PIN;      // ( Pin 33 para YL69 )
  const unsigned short int ldrPin = LDR_PIN;      // ( Pin 34 para LDR )
  const unsigned short int mqPin = MQ135_AO_PIN;  // ( Pin 35 para MQ135 )

  // Instancias
  SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
  static DHT dht(dhtPin, dhtType);
  static MQUnifiedsensor mqSensor("ESP-32", MQ135_VOLTAGE, MQ135_ADC_RESOLUTION, MQ135_AO_PIN, "MQ-135");

  // Inicializar
  Serial.begin(115200);
  dht.begin();

  // Parámetros para tasks
  void *paramData[1] = {mutex}; 
  void *paramsDHT[2] = {mutex, &dht};
  void *paramsMQ135[2] = {mutex, &mqSensor};
  void *paramsLDR[2] = {mutex, (void *)(uintptr_t)ldrPin};
  void *paramsYL69[2] = {mutex, (void *)(uintptr_t)ylPin};

  // Tasks Sensores
  xTaskCreate(taskEspNow, "ESP-NOW Task", 8196, paramData, 2, NULL);
  xTaskCreate(taskTemperature, "DHT11: Read Temperature Function", 4096, paramsDHT, 2, NULL);
  xTaskCreate(taskHumidity, "DHT11: Read Humidity Function", 4096, paramsDHT, 2, NULL);
  xTaskCreate(taskMQ135, "MQ135: Read Gas Concentration", 2048, paramsMQ135, 2, NULL);
  xTaskCreate(taskYL69, "YL69: Read Soil Moisture", 2048, paramData, 2, NULL);
  xTaskCreate(taskLDR, "LDR: Read Light Level", 2048, paramsLDR, 2, NULL);
  // Tarea para imprimir datos
  //xTaskCreate(taskPrintData, "Print Sensor Data", 2048, paramData, 2, NULL);
}

void loop() {}