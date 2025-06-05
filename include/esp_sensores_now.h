#ifndef ESP_MAIN_NOW_H
#define ESP_MAIN_NOW_H

// Librerías necesarias
#include <freertos/semphr.h>
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "sensorData.h"
#include "esp_wifi.h"

// Definiciones
#define DELAY_ESP_NOW 250 ///< Tiempo de espera entre envíos de datos a través de ESP-NOW
const uint8_t BROADCAST_ADDRESS[] = {0xF4, 0x65, 0x0B, 0xE5, 0x49, 0x88};
extern SemaphoreHandle_t mutex;

/**
 * @file 
 * @brief 
 *
 * @param pvParameters 
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "📩 Entrega Exitosa a ESPmain" : "⚠️ Error de Entrega a ESPmain");
}
esp_err_t esp_wifi_config_espnow_channel(uint8_t channel) {
    esp_err_t err = esp_wifi_set_promiscuous(true);
    if (err != ESP_OK) {
        return err;
    }
    err = esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    if (err != ESP_OK) {
        return err;
    }
    err = esp_wifi_set_promiscuous(false);
    return err;
}
void taskEspNow(void *pvParameters){    
    // Inicializar ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // Registrar callback para el estado del envío
    esp_err_t result = esp_now_register_send_cb(OnDataSent);
    if (result != ESP_OK) {
        Serial.print("Error registering send callback: ");
        Serial.println(esp_err_to_name(result));
        return;
    }


    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, BROADCAST_ADDRESS, 6);
    peerInfo.channel = 5;  
    peerInfo.encrypt = false;
esp_wifi_config_espnow_channel(5);
    // Agregar par
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }

    while (true) {
        // Enviar datos a través de ESP-NOW
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            buffer = data;
            xSemaphoreGive(mutex);
        }

        esp_err_t result = esp_now_send(BROADCAST_ADDRESS, (uint8_t *) &buffer, sizeof(buffer));
        if (result == ESP_OK) {
            Serial.println("📤 Data enviada correctamente a ESPmain");
        } else {
            Serial.print("⚠️ Error de envío a ESPmain: ");
            Serial.println(esp_err_to_name(result));
        }

        // Esperar un tiempo antes de enviar nuevamente
        vTaskDelay(pdMS_TO_TICKS(DELAY_ESP_NOW));
    }
}

#endif