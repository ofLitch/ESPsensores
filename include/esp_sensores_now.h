#ifndef ESP_MAIN_NOW_H
#define ESP_MAIN_NOW_H

// Librerías necesarias
#include <freertos/semphr.h>
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "sensorData.h"

// Definiciones
#define DELAY_ESP_NOW 2000 ///< Tiempo de espera entre envíos de datos a través de ESP-NOW
const uint8_t BROADCAST_ADDRESS[] = {0xF4, 0x65, 0x0B, 0xE5, 0x49, 0x88};

/**
 * @file 
 * @brief 
 *
 * @param pvParameters 
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void taskEspNow(void *pvParameters){
    // Parámetros de entrada
    SemaphoreHandle_t mutex = ((SemaphoreHandle_t *)pvParameters)[0];
    
    // Inicializar ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // Registrar callback para el estado del envío
    esp_now_register_send_cb(OnDataSent);
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, BROADCAST_ADDRESS, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Agregar par
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
    SensorData buffer;
    buffer.ppm = 0.0f;
    buffer.temperature = 0.0f;  
    buffer.humidity = 0.0f;
    buffer.soilWet = 0.0f;
    buffer.light = 0;
    buffer.dateTime = RtcDateTime(0); // Inicializar con 0 segundos desde 2000
    while (true) {
        // Enviar datos a través de ESP-NOW
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            buffer.ppm = data.ppm;              
            buffer.temperature = data.temperature;   
            buffer.humidity = data.humidity;           
            buffer.soilWet = data.soilWet;            
            buffer.light = data.light;     
            buffer.dateTime = RtcDateTime(0); // Inicializar con 0 segundos desde 2000
            xSemaphoreGive(mutex);
        }

        esp_err_t result = esp_now_send(BROADCAST_ADDRESS, (uint8_t *) &buffer, sizeof(buffer));
        if (result == ESP_OK) {
            Serial.println("Data sent successfully");
        } else {
            Serial.print("Error sending data: ");
            Serial.println(esp_err_to_name(result));
        }

        // Esperar un tiempo antes de enviar nuevamente
        vTaskDelay(pdMS_TO_TICKS(DELAY_ESP_NOW));
    }
}

#endif