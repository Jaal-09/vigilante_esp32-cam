#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <esp_http_server.h>
#include "CamaraManager.h"

class ServerManager {
private:
    httpd_handle_t serverHandle;
    CamaraManager* camaraRef;

    static esp_err_t streamHandler(httpd_req_t* req);
    static esp_err_t indexHandler(httpd_req_t* req);
    static esp_err_t fotoAlertaHandler(httpd_req_t* req); // Manejador de la foto fija

public:
    ServerManager(CamaraManager* camara);
    void iniciar();
};

#endif