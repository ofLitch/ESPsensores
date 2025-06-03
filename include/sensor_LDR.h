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
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    int ldrPin = (int)(uintptr_t)params[1];

    // Comprobar que el mutex y el pin LDR sean válidos
    if (mutex == NULL) {
        Serial.println("Error: Mutex no inicializado en la tarea LDR.");
        vTaskDelete(NULL); // Terminar la tarea si el mutex es inválido
        return;
    }
    if (ldrPin < 30 || ldrPin > 39) {
        Serial.println("Error: Pin LDR inválido.");
        vTaskDelete(NULL); // Terminar la tarea si el pin es inválido
        return;
    }

    // Configuración del LDR
    pinMode(ldrPin, INPUT);
    unsigned short int light = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Comenzar la lectura del LDR
    while (true) {
        light = analogRead(ldrPin);
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            data.light = light;
            xSemaphoreGive(mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(LDR_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_LDR_H
