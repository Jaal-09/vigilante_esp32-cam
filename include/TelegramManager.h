#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "CamaraManager.h"

class TelegramManager {
private:
    const char* tokenBot;
    const char* chatID;
    WiFiClientSecure clientSecure;

public:
    TelegramManager(const char* token, const char* id);
    void iniciar();
    bool enviarAlertaConFoto(String mensaje, camera_fb_t* fb);
};

#endif