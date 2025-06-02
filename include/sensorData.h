#ifndef SENSORDATA_H
#define SENSORDATA_H

// Estructura de datos compartidos
struct SensorData {
  float temperature;        ///< Temperatura medida por el sensor DHT
  unsigned short humidity;  ///< Humedad medida por el sensor DHT
  unsigned short light;     ///< Nivel de luz medido por el sensor LDR
  RtcDateTime dateTime;     ///< Hora leída por el DS1302
}data;

#endif // SENSORDATA_H