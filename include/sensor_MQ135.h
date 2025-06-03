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
 * @file sensor_MQ135.h
  * @brief Definición de la tarea para leer datos del sensor MQ135 y actualizar los datos compartidos.
 *
 * @param pvParameters Puntero a los parámetros que incluye el mutex y el sensor MQ135.
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
    if( mutex == NULL ) { 
        Serial.println("Error: Mutex no inicializado en la tarea MQ135.");
        mqSensor->serialDebug();
        vTaskDelete(NULL);
        return;
    }
    if( mqSensor->getR0() <= 0 || mqSensor->getR0() > 1000000  || isinf(mqSensor->getR0())) {
        Serial.println("Error: R0 inválido en la tarea MQ135.");
        mqSensor->serialDebug();
        vTaskDelete(NULL);
        return;
    }

    // Comenzar la lectura del sensor MQ135
    float ppm = 0.0f;
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    while (true) {
        mqSensor->update();
        ppm = mqSensor->readSensor();
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            data.ppm = ppm;
            xSemaphoreGive(mutex);
        }
        // mqSensor.serialDebug();

        vTaskDelay(pdMS_TO_TICKS(MQ135_READ_INTERVAL_MS));
    }
}

#endif // SENSOR_MQ135_H
