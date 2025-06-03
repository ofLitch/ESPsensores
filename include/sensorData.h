#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <RtcDS1302.h>

/**
 * @brief Estructura para almacenar datos de los sensores ambientales.
 *
 * Contiene los valores medidos de temperatura, humedad, luz, humedad del suelo, concentración de gases y la fecha/hora actual.
 */
struct SensorData {
    float ppm;                ///< Concentración de gases CO2 medida por el sensor MQ135
    float temperature;        ///< Temperatura medida por el sensor DHT
    float humidity;           ///< Humedad medida por el sensor DHT
    float soilWet;            ///< Humedad del suelo medida por el sensor YL69
    unsigned short light;     ///< Nivel de luz medido por el sensor LDR
    RtcDateTime dateTime;     ///< Hora leída por el DS1302
} data;

extern SensorData data;

#endif // SENSORDATA_H