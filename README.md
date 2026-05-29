# 🚨 Sistema de Vigilancia con ESP32-CAM y Alertas en Telegram (POO)

Este proyecto consiste en un sistema de seguridad automatizado y modular desarrollado para la plataforma **ESP32-CAM** bajo el paradigma de **Programación Orientada a Objetos (POO)** en el entorno **PlatformIO**. El sistema utiliza un sensor de movimiento PIR para detectar intrusos, captura una instantánea en tiempo real y la transmite instantáneamente a un chat privado de **Telegram** utilizando sockets TCP y peticiones HTTP POST (Multipart/form-data) directas, optimizando el uso de la memoria RAM del microcontrolador.

---

## 🚀 Características Principales

* **Arquitectura POO Estricta:** Código completamente modular dividido en mánagers especializados (`Wifi`, `Camara`, `Telegram`).
* **Transmisión Binaria Directa:** Envío de fotos desde la memoria RAM (*heap*) del ESP32 a la API de Telegram sin depender de almacenamiento local (MicroSD) o servidores intermediarios.
* **Optimización de Memoria:** Implementación de conexiones SSL Inseguras (`clientSecure.setInsecure()`) para omitir la carga de certificados pesados, evitando reinicios por *Kernel Panic* o falta de memoria.
* **Filtro Anti-Spam (Respiro):** Temporización inteligente mediante `millis()` que bloquea alertas consecutivas durante un intervalo de 20 segundos tras una detección exitosa.
* **Cero Bloqueos:** Eliminación de servidores de streaming en vivo concurrentes para garantizar una estabilidad del 100% en las notificaciones críticas.

---

## 🛠️ Arquitectura del Software (Módulos POO)

El código fuente se encuentra estructurado de forma limpia dentro de la carpeta `src/`, separando las responsabilidades de la siguiente manera:

* **`main.cpp`**: El cerebro del sistema. Inicializa los periféricos, coordina los módulos y ejecuta el ciclo de lectura del sensor PIR (`loop`).
* **`WifiManager`**: Encargado de gestionar la conexión física a la red local inalámbrica de forma asíncrona.
* **`CamaraManager`**: Encapsula las configuraciones del sensor de imagen OV2640, el tamaño del frame (JPEG) y los métodos de captura/liberación de memoria.
* **`TelegramManager`**: Implementa el protocolo de red HTTP para empaquetar la imagen y el texto en formato `multipart/form-data` hacia los servidores de Telegram.

---

## 📋 Requisitos de Hardware

1.  Módulo **ESP32-CAM** (Ai-Thinker).
2.  Convertidor FTDI (USB a Serial) para la programación.




## 💻 Configuración e Instalación

### 1. Clonar el repositorio
```bash
git clone [https://github.com/TU_USUARIO/esp32cam-telegram-security.git](https://github.com/TU_USUARIO/esp32cam-telegram-security.git)
cd esp32cam-telegram-security

```

## 2. Configurar las credenciales

```bash

const char* WIFI_SSID = "Tu_Nombre_De_Red";
const char* WIFI_PASS = "Tu_Contraseña";

// Credenciales obtenidas vía @BotFather y @myidbot en Telegram
const char* TELEGRAM_TOKEN = "1234567890:ABCdefGhIJKlmNoPQRsTUVwxyZ"; 
const char* TELEGRAM_CHAT_ID = "123456789";

```


## 🛠️ Tecnologías Utilizadas
Lenguaje: C++ 

Entorno de Desarrollo: VS Code + PlatformIO

Framework: Arduino Core para ESP32

Protocolos: HTTPS, TCP/IP, HTTP Multipart Form Data

## ✒️ Desarrollado por: [Jesús Arias / Estudiante de Ingeniería Mecatrónica] - Universidad de Pamplona.