#ifndef SENSOR_MQ135_H
#define SENSOR_MQ135_H

// Librerías necesarias
#include "MQUnifiedsensor.h"
#include "sensorData.h"

// Definiciones para el sensor MQ135
#define MQ135_READ_INTERVAL_MS 1000               ///< Intervalo de muestreo en milisegundos
#define MQ135_RL 10.0f                            ///< Resistencia de carga (RL) en kiloohmios
#define MQ135_A 110.47                            ///< Valor A para configurar CO2
#define MQ135_B -2.862                            ///< Valor B para configurar CO2

/**
 * @brief Tarea encargada de leer el sensor MQ135 de gases y mostrar valores procesados por consola.
 * 
 * Esta tarea ejecuta lecturas periódicas del sensor, calcula la resistencia, R0 y PPM
 * tanto corregido como sin corregir, usando temperatura y humedad estimadas.
 * 
 * @param pvParameters Puntero a parámetros si se usan datos compartidos o mutex.
 */
void taskMQ135(void *pvParameters) {    
    // Desempaquetar los parámetros
    void **params = (void **)pvParameters;
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)params[0];
    MQUnifiedsensor* mqSensor = (MQUnifiedsensor *)params[1];

    // Inicializar el sensor MQ135
    mqSensor->setRegressionMethod(1); //_PPM =  a*ratio^b
    mqSensor->setRL(MQ135_RL); // Configurar resistencia de carga (RL)
    mqSensor->setA( MQ135_A ); mqSensor->setB( MQ135_B ); // Valores para configurar CO2
    mqSensor->init();
    vTaskDelay(pdMS_TO_TICKS(1000)); 
    
    // Calibrar el sensor MQ135
    mqSensor->serialDebug(true);
    float calcR0 = 0;
    for(int i = 1; i<=10; i ++)    {
        mqSensor->update();
        calcR0 += mqSensor->calibrate(3.6);
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
    mqSensor->setR0(calcR0/10);

    // Comprobar que todos los parámetros sean válidos
    if( mutex == NULL || isinf(calcR0) || calcR0 == 0 ) { 
        Serial.println("Error: Parámetros inválidos en la tarea MQ135.");
        mqSensor->serialDebug();
        vTaskDelete(NULL); // Terminar la tarea si los parámetros son inválidos
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(3000));
    while (true) {
        mqSensor->update();
        float ppm = mqSensor->readSensor();
        Serial.print("PPM: ");
        Serial.println(ppm);
        // mqSensor.serialDebug();

        vTaskDelay(pdMS_TO_TICKS(MQ135_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_MQ135_H
