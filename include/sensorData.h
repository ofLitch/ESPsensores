#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <RtcDS1302.h>

/**
 * @file sensorData.h
 * @brief Definición de la estructura SensorData para almacenar datos de sensores ambientales.
 * 
 * Contiene los valores medidos de temperatura, humedad, luz, humedad del suelo, concentración de gases y la fecha/hora actual.
 */
struct SensorData {
    float ppm = 0.0f;                ///< Concentración de gases CO2 medida por el sensor MQ135
    float temperature = 0.0f;        ///< Temperatura medida por el sensor DHT
    float humidity = 0.0f;           ///< Humedad medida por el sensor DHT
    float soilWet = 0.0f;            ///< Humedad del suelo medida por el sensor YL69
    unsigned short light = 0;       ///< Nivel de luz medido por el sensor LDR
    RtcDateTime dateTime = RtcDateTime(0);     ///< Hora leída por el DS1302
} data;

extern SensorData data;

#endif // SENSORDATA_H