#include <Arduino.h>
#include "WifiManager.h"
#include "CamaraManager.h"
#include "TelegramManager.h"

// Configuraciones de credenciales
const char* WIFI_SSID = "Wifi";
const char* WIFI_PASS = "contraseña";

// Credenciales de Telegram
const char* TELEGRAM_TOKEN = "Token";
const char* TELEGRAM_CHAT_ID = "ID_telegram";

const int PIN_SENSOR_MOVIMIENTO = 13; // Pin del sensor PIR

// Instanciamos nuestros objetos globales (Estructura POO Limpia)
WifiManager wifi(WIFI_SSID, WIFI_PASS);
CamaraManager camara;
TelegramManager telegram(TELEGRAM_TOKEN, TELEGRAM_CHAT_ID); // <--- NUEVO

unsigned long tiempoUltimaAlerta = 0;
const unsigned long ESPERA_ENTRE_ALERTAS = 20000; // 20 segundos de respiro

void setup() {
    Serial.begin(115200);
    pinMode(PIN_SENSOR_MOVIMIENTO, INPUT);
    
    // Inicializar módulos de comunicación
    wifi.conectar();
    
    // Inicializar el hardware de la cámara
    if(camara.iniciar()) {
        Serial.println("¡Hardware de Cámara configurado!");
        // Inicializar módulo de Telegram (SSL Insecure)
        telegram.iniciar(); 
    } else {
        Serial.println("Fallo crítico en el hardware de la cámara.");
    }
    
    Serial.println("Sistema de Vigilancia Telegram POO Activo.");
}

void loop() {
    // Si el sensor detecta presencia
    if (digitalRead(PIN_SENSOR_MOVIMIENTO) == HIGH && (millis() - tiempoUltimaAlerta >= ESPERA_ENTRE_ALERTAS)) {
        
        Serial.println("🚨 ¡MOVIMIENTO DETECTADO! Capturando evidencia real...");
        tiempoUltimaAlerta = millis(); // Actualizar tiempo para bloqueo de spam
        
        // 1. Capturar foto INMEDIATAMENTE
        camera_fb_t* foto = camara.capturarFoto();
        
        if(!foto) {
            Serial.println("Error físico al capturar el cuadro de la cámara.");
        } else {
            Serial.println("Foto resguardada en RAM (heap). Enviando a Telegram...");
            
            // 2. Definir el mensaje de texto que acompaña la foto
            String mensaje = "⚠️ ¡ALERTA DE INTRUSIÓN! Se ha detectado movimiento en la zona de vigilancia. Adjuntando captura de pantalla en tiempo real.";
            
            // 3. Llamar al método estrella: Pasa el texto y el PUNTERO de la foto
            // Telegram subirá los bytes directos.
            telegram.enviarAlertaConFoto(mensaje, foto);
            
            // 4.Liberar la memoria RAM de la cámara inmediatamente
            camara.liberarFoto(foto); 
        }
        Serial.println("Ciclo de evento de seguridad cerrado. Entrando en respiro.");
    }
    
    // delay en el loop principal.no hay video en tiempo real video,
    // pero asi el procesador está casi libre, consume menos.
    delay(50); 
}