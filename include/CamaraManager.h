#ifndef CAMARA_MANAGER_H
#define CAMARA_MANAGER_H

#include "esp_camera.h"
#include <Arduino.h>

class CamaraManager {
private:
    camera_config_t config;

public:
    CamaraManager();
    bool iniciar();
    camera_fb_t* capturarFoto();
    void liberarFoto(camera_fb_t* fb);
};

#endif