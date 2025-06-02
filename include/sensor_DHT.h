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
    unsigned short int dhtPin = *((unsigned short int*)params[1]);
    unsigned short int dhtType = *((unsigned short int*)params[2]);

    // Inicializar el sensor DHT - ¡Cada tarea necesita su propia instancia!
    DHT dht(dhtPin, dhtType);
    dht.begin();

    // Comprobar que todos los parámetros sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Parámetros inválidos en la tarea de temperatura.");
        vTaskDelete(NULL);
        return;
    }

    while (true) {
        float temperature = dht.readTemperature();
        if (!isnan(temperature)) {
            if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                data.temperature = temperature;
                xSemaphoreGive(mutex);
            }
        } else {
            Serial.println("Error leyendo temperatura. Usando valor anterior.");
        }

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
    unsigned short int dhtPin = *((unsigned short int*)params[1]);
    unsigned short int dhtType = *((unsigned short int*)params[2]);

    // Inicializar el sensor DHT - ¡Cada tarea necesita su propia instancia!
    DHT dht(dhtPin, dhtType);
    dht.begin();

    // Comprobar que todos los parámetros sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Parámetros inválidos en la tarea de humedad.");
        vTaskDelete(NULL);
        return;
    }

    while (true) {
        float humidity = dht.readHumidity();
        if (!isnan(humidity)) {
            if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                data.humidity = humidity;
                xSemaphoreGive(mutex);
            }
        } else {
            Serial.println("Error leyendo humedad. Usando valor anterior.");
        }

        vTaskDelay(pdMS_TO_TICKS(DHT_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_DHT_H