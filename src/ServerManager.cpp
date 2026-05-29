#include "ServerManager.h"
#include <Arduino.h>

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static CamaraManager* globalCamaraRef = nullptr;

// Variables globales para almacenar temporalmente la foto de la alerta PIR
uint8_t* bufferFotoAlerta = nullptr;
size_t longitudFotoAlerta = 0;

ServerManager::ServerManager(CamaraManager* camara) {
    this->serverHandle = NULL;
    this->camaraRef = camara;
    globalCamaraRef = camara;
}

esp_err_t ServerManager::indexHandler(httpd_req_t* req) {
    const char* html = "<html><head><title>ESP32-CAM POO</title></head>"
                       "<body style='background-color:#222; color:white; text-align:center; font-family:Arial;'>"
                       "<h1>Sistema de Seguridad Avanzado V2</h1>"
                       "<p>Monitoreo en vivo y alertas con captura a WhatsApp</p>"
                       "<img src='/stream' style='border:4px solid #fff; border-radius:8px; max-width:100%;'>"
                       "</body></html>";
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html, strlen(html));
}

esp_err_t ServerManager::fotoAlertaHandler(httpd_req_t* req) {
    if (bufferFotoAlerta == nullptr || longitudFotoAlerta == 0) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    // Entregamos los bytes puros guardados en RAM al servidor
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=alerta.jpg");
    return httpd_resp_send(req, (const char*)bufferFotoAlerta, longitudFotoAlerta);
}

esp_err_t ServerManager::streamHandler(httpd_req_t* req) {
    esp_err_t res = ESP_OK;
    char* part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) return res;

    while (true) {
        if (globalCamaraRef == nullptr) break;

        camera_fb_t* fb = globalCamaraRef->capturarFoto();
        if (!fb) {
            res = ESP_FAIL;
            break;
        }

        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res != ESP_OK) {
            globalCamaraRef->liberarFoto(fb);
            break;
        }

        size_t hlen = snprintf((char*)part_buf, 64, _STREAM_PART, fb->len);
        res = httpd_resp_send_chunk(req, (const char*)part_buf, hlen);
        if (res != ESP_OK) {
            globalCamaraRef->liberarFoto(fb);
            break;
        }

        res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
        globalCamaraRef->liberarFoto(fb);
        if (res != ESP_OK) break;
        
        // Subimos de 40 a 80 para darle tiempo al loop principal
        // de enterarse cuando el sensor PIR detecte movimiento físico.
        delay(80); 
    }
    return res;
}

void ServerManager::iniciar() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    //No esta implementado como tal
    if (httpd_start(&serverHandle, &config) == ESP_OK) {
        // Registro de la Página de Inicio
        httpd_uri_t index_uri = {.uri = "/", .method = HTTP_GET, .handler = indexHandler, .user_ctx = NULL};
        httpd_register_uri_handler(serverHandle, &index_uri);

        // Registro del Streaming de Video
        httpd_uri_t stream_uri = {.uri = "/stream", .method = HTTP_GET, .handler = streamHandler, .user_ctx = NULL};
        httpd_register_uri_handler(serverHandle, &stream_uri);

        // Registro de la foto estática de Alerta
        httpd_uri_t foto_uri = {.uri = "/alerta.jpg", .method = HTTP_GET, .handler = fotoAlertaHandler, .user_ctx = NULL};
        httpd_register_uri_handler(serverHandle, &foto_uri);
        
        Serial.println("¡Servidor Web Modular iniciado correctamente!");
    }
}