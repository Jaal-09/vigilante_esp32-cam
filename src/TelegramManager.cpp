#include "TelegramManager.h"

TelegramManager::TelegramManager(const char* token, const char* id) {
    this->tokenBot = token;
    this->chatID = id;
}

void TelegramManager::iniciar() {
    // Saltamos la verificación estricta de certificados para liberar memoria RAM
    this->clientSecure.setInsecure();
    Serial.println("Módulo de Telegram HTTP activo (Insecure SSL).");
}

bool TelegramManager::enviarAlertaConFoto(String mensaje, camera_fb_t* fb) {
    if (fb == nullptr) return false;

    Serial.println("Conectando directo con servidores de Telegram...");
    
    // Conexión directa por puerto seguro HTTPS (443)
    if (!clientSecure.connect("api.telegram.org", 443)) {
        Serial.println("Error: No se pudo conectar a api.telegram.org");
        return false;
    }

    Serial.println("¡Conectado! Construyendo formulario para la foto...");

    String boundary = "---------------------------ESP32CAMVIGILANTEPOO";
    String cabeceraChat = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"chat_id\"\r\n\r\n" + String(chatID) + "\r\n";
    String cabeceraTexto = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"caption\"\r\n\r\n" + mensaje + "\r\n";
    String cabeceraFoto = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"alerta.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String colaFormulario = "\r\n--" + boundary + "--\r\n";

    // Calculamos el tamaño total en bytes del paquete de red
    size_t longitudTotal = cabeceraChat.length() + cabeceraTexto.length() + cabeceraFoto.length() + fb->len + colaFormulario.length();

    // Enviamos la petición POST cruda por TCP
    clientSecure.print("POST /bot" + String(tokenBot) + "/sendPhoto HTTP/1.1\r\n");
    clientSecure.print("Host: api.telegram.org\r\n");
    clientSecure.print("Content-Type: multipart/form-data; boundary=" + boundary + "\r\n");
    clientSecure.print("Content-Length: " + String(longitudTotal) + "\r\n\r\n");

    // Inyectamos las secciones del formulario de texto
    clientSecure.print(cabeceraChat);
    clientSecure.print(cabeceraTexto);
    clientSecure.print(cabeceraFoto);

    // Transmitimos los bytes binarios de la imagen en fragmentos de 1KB para estabilidad
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    const size_t tamanoBufferEnvio = 1024;
    
    for (size_t i = 0; i < fbLen; i += tamanoBufferEnvio) {
        size_t tamanoActual = (fbLen - i < tamanoBufferEnvio) ? (fbLen - i) : tamanoBufferEnvio;
        clientSecure.write(fbBuf + i, tamanoActual);
    }

    // Cerramos el envío del formulario
    clientSecure.print(colaFormulario);
    Serial.println("Foto enviada. Esperando respuesta del servidor...");
    
    unsigned long timeout = millis();
    while (clientSecure.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println("Error: Timeout de respuesta de Telegram.");
            clientSecure.stop();
            return false;
        }
        delay(10);
    }

    String respuesta = clientSecure.readStringUntil('\r');
    clientSecure.stop();

    if (respuesta.indexOf("200 OK") != -1) {
        Serial.println("¡Alerta con foto enviada con éxito total!");
        return true;
    } else {
        Serial.println("Telegram denegó el acceso. HTTP status: " + respuesta);
        return false;
    }
}