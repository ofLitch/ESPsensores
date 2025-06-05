#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

// Librerías necesarias
#include "DHT.h"
#include "sensorData.h"
#define DHT_READ_INTERVAL_MS 2100  ///< Intervalo de muestreo en milisegundos
extern SemaphoreHandle_t mutex;

/**
 * @file sensor_DHT.h
 * @brief Definición de las tareas para leer datos del sensor DHT y actualizar los datos compartidos.
 *
 * @param pvParameters Puntero a los parámetros que incluye el mutex y el sensor DHT.
 */
void taskTemperature(void *pvParameters) {
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    DHT *dht = (DHT *)params[0];

    // Comprobar que el mutex y el sensor DHT sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Mutex no inicializado.");
        vTaskDelete(NULL);
        return;
    }

    // Configurar el sensor DHT y comenzar la lectura
    float temperature = 0.0f;
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (true) {
        temperature = dht->readTemperature();
        if (!isnan(temperature)) {
            if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                data.temperature = temperature;
                xSemaphoreGive(mutex);
            }
        } else Serial.println("Error leyendo temperatura. Usando valor anterior.");
        
        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
    }
}

/**
 * @brief Lee la humedad desde el sensor DHT y actualiza los datos compartidos.
 *
 * @param pvParameters Puntero a los parámetros que incluye el mutex y el sensor DHT.
 */
void taskHumidity(void *pvParameters) {
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    DHT *dht = (DHT *)params[0];

    // Comprobar que el mutex y el sensor DHT sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Mutex no inicializado.");
        vTaskDelete(NULL);
        return;
    }

    // Configurar el sensor DHT y comenzar la lectura
    float humidity = 0.0f;
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    while (true) {
        humidity = dht->readHumidity();
        if (!isnan(humidity)) {
            if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                data.humidity = humidity;
                xSemaphoreGive(mutex);
            }
        } else Serial.println("Error leyendo humedad. Usando valor anterior");
        
        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS+205));
    }
}

#endif // SENSOR_DHT_H