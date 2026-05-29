#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WifiManager {
private:
    const char* ssid;
    const char* password;

public:
    WifiManager(const char* ssidAsignado, const char* passAsignado);
    void conectar();
    bool estaConectado();
};

#endif