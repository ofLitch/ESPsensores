#ifndef SENSORDATA_H
#define SENSORDATA_H

// Estructura de datos compartidos
struct SensorData {
  float ppm;                ///< Concentración de gases CO2 medida por el sensor MQ135
  float temperature;        ///< Temperatura medida por el sensor DHT
  float humidity;           ///< Humedad medida por el sensor DHT
  float soilWet;            ///< Humedad del suelo medida por el sensor YL69
  unsigned short light;     ///< Nivel de luz medido por el sensor LDR
  RtcDateTime dateTime;     ///< Hora leída por el DS1302
} data;

#endif // SENSORDATA_H