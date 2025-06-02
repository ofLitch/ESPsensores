#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

// Librerías necesarias
#include "DHT.h"
#include "sensorData.h"
#define DHT_READ_INTERVAL_MS 2100  ///< Intervalo de muestreo en milisegundos

/**
 * @brief Lee la temperatura desde el sensor DHT y actualiza los datos compartidos.
 *
 * @param pvParameters Puntero a los parámetros que incluye datos compartidos, sensor y mutex.
 */
void taskTemperature(void *pvParameters) {
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    DHT *dht = (DHT *)params[1];

    while (true) {
        float temperature = dht->readTemperature();
        Serial.print("Temperatura: ");
        Serial.println(temperature);
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
 * @param pvParameters Puntero a los parámetros que incluye datos compartidos, sensor y mutex.
 */
void taskHumidity(void *pvParameters) {
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    DHT *dht = (DHT *)params[1];

    while (true) {
        float humidity = dht->readHumidity();
        Serial.print("Humedad: ");
        Serial.println(humidity);
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