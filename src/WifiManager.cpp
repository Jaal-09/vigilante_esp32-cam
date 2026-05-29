#include "WifiManager.h"

WifiManager::WifiManager(const char* ssidAsignado, const char* passAsignado) {
    ssid = ssidAsignado;
    password = passAsignado;
}

void WifiManager::conectar() {
    Serial.print("Conectando a ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n¡Wi-Fi Conectado exitosamente!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

bool WifiManager::estaConectado() {
    return (WiFi.status() == WL_CONNECTED);
}