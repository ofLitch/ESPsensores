#ifndef SENSOR_LDR_H
#define SENSOR_LDR_H

// Librerías necesarias
#include "sensorData.h"
#include <freertos/semphr.h>
#include <Arduino.h>

// Definiciones para el sensor LDR
#define LDR_READ_INTERVAL_MS 1000               ///< Intervalo de muestreo en milisegundos

/**
 * @brief Lee el nivel de luz desde el sensor LDR y actualiza los datos compartidos.
 * 
 * @param pvParameters Puntero a los parámetros que incluye datos compartidos y mutex.
 */
void taskLDR(void *pvParameters) {    
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    int ldrPin = (int)(uintptr_t)params[1];
    pinMode(ldrPin, INPUT); // Configurar el pin LDR como entrada

    while (true) {
        data.light = analogRead(ldrPin);
        Serial.print("LDR: ");
        Serial.println(data.light);
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            xSemaphoreGive(mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(LDR_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_LDR_H
