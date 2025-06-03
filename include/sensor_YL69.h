#ifndef SENSOR_YL69_H
#define SENSOR_YL69_H

// Librerías necesarias
#include "sensorData.h"
#include <freertos/semphr.h>
#include <Arduino.h>

// Definiciones para el sensor YL69
#define YL_READ_INTERVAL_MS 3000               ///< Intervalo de muestreo en milisegundos

/**
 * @file sensor_YL69.h
  * @brief Definición de la tarea para leer datos del sensor YL69 y actualizar los datos compartidos.
 *
 * @param pvParameters Puntero a los parámetros que incluye el mutex y el pin del sensor YL69.
 */
void taskYL69(void *pvParameters) {
    // Desempaquetar los parámetros    
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    int ylPin = (int)(uintptr_t)params[1];

    // Comprobar que el mutex y el pin YL69 sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Mutex no inicializado en la tarea YL69.");
        vTaskDelete(NULL);
        return;
    }
    if (ylPin < 30 || ylPin > 39) {
        Serial.println("Error: Pin YL69 inválido.");
        vTaskDelete(NULL);
        return;
    }

    // Configuración del YL69 y comienzo de la lectura
    pinMode(ylPin, INPUT);
    float soilWet = 0.0f;
    
    while (true) {
        soilWet = analogRead(ylPin);
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            data.soilWet = soilWet;
            xSemaphoreGive(mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(YL_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_YL69_H
