# AntipanicButton

#Botón de Pánico con Alerta Visual, Sonora y Comunicación Multicanal

Este proyecto implementa un sistema de alerta de emergencia basado en una placa **LilyGO T3 V1.6.1** con conectividad **LoRaWAN**, **Bluetooth** y módulos periféricos. El objetivo principal es permitir que una persona en situación de peligro pueda enviar una señal de alerta que llegue a un servidor remoto, el cual procesará la información y se encargará de notificar a familiares o servicios de ayuda a través de **WhatsApp, SMS** y señales de retorno visuales y sonoras.

---

## 📟 Frontend (Código del Dispositivo)

El código implementado en la LilyGO está desarrollado en **Arduino IDE** y maneja distintos módulos de hardware, cada uno con su función dedicada:

### Módulos principales gestionados:
- **GPS (NEO-6M):** obtiene las coordenadas tras una alerta. El sistema espera 2 segundos para asegurar que los datos estén disponibles y estables.
- **Acelerómetro (MPU6050):** detecta caídas. Su sensibilidad ha sido calibrada para reducir falsos positivos.
- **DFPlayer Mini:** reproduce archivos de audio en formato MP3 almacenados en una tarjeta microSD formateada en FAT32. Se utilizan nombres tipo `0001.mp3`, `0002.mp3`, etc.
- **LED bicolor:** indica el estado de la alerta (rojo para enviada, verde fijo para confirmada, verde intermitente para ayuda en camino).
- **Pulsador:** activa el sistema y genera el envío de alerta.
- **Bluetooth (en caso de fallo LoRa):** si no se completa la transmisión por LoRa (evento `EV_JOIN_FAILED` o sin `tx_complete`), el dispositivo envía un mensaje `"PANIC_ALERT"` al móvil vía Bluetooth clásico para que este se encargue del reenvío del mensaje por SMS mediante Automate.

---

## 🌐 Backend (Servidor Local)

El backend está desarrollado en **Node.js** y corre de forma **local en el ordenador del operador**. Para poder recibir datos desde The Things Stack (TTN), se emplea **Ngrok**, que permite exponer el servidor local a Internet mediante una URL pública segura por HTTPS.

> **⚠️ Requisitos:**  
> - Tener **Node.js** instalado.  
> - Ejecutar `npm install` para instalar todas las dependencias (`express`, `axios`, `dotenv`, etc.).
> - Ejecutar server con node server.js
> - El código está programado para el puerto 3000 del localhost. Para exponerlo con Ngrok ejecutar el comando Ngrok htttp 3000 y obtener la url pública que Ngrok proporciona. Está url pública ha de ser sustituida en el webhook del proyecto (que ha de ser programado en TTN en caso de resetearse) y en el archivo index.html.
> - Tener un archivo `.env` con las claves y tokens necesarios (Twilio, ClickSend, TTN API...).

---

## 🧠 Funciones del Backend

### `sendDownlink`
Genera y envía un **mensaje de respuesta (downlink)** a la LilyGO a través de la API de TTN. Se utiliza para controlar acciones en el dispositivo como encender LEDs o reproducir audios.

### `operatorCommand`
Gestiona los comandos enviados desde una interfaz web (`index.html`). Al introducir un código (como `"02"`), este se traduce a un payload y se envía como downlink al dispositivo.

### `sendSMS`
Envía un **SMS automático** usando la API de ClickSend cuando se recibe una alerta. Útil para notificar a familiares o cuidadores.

### `sendWhatsapp`
Envía un **mensaje de WhatsApp** usando la API de Twilio. Se utiliza para avisar con rapidez tras una pulsación del botón de pánico.

---

## 🖥️ Interfaces Web

### `index.html`
Interfaz sencilla en la que el operador puede introducir un código que representa el estado de la alerta. Por ejemplo:
- `"02"`: alerta confirmada, ayuda en camino (LED verde fijo).
- `"03"`: ayuda confirmada, LED verde intermitente + audio de "la ayuda está en camino".

### `mapa.html`
Interfaz que muestra sobre un mapa la **ubicación GPS** de la persona que ha activado la alerta, usando **Leaflet.js**. Se actualiza automáticamente cada vez que llega una nueva alerta.

---

## 🔐 Seguridad

- Todas las claves (Twilio, ClickSend, TTN...) se almacenan en un archivo `.env`, nunca en el código fuente.
- Las comunicaciones entre TTN, Ngrok y el servidor se realizan por **HTTPS**.
- Se recomienda proteger `index.html` con autenticación básica si se expone públicamente.
- La app Automate en el móvil del usuario no requiere interacción: actúa automáticamente al recibir la señal Bluetooth si no hay cobertura LoRa.

---

## 📲 Comunicación Bluetooth de Respaldo

En caso de que no haya confirmación de transmisión por LoRa:
- El dispositivo envía `"PANIC_ALERT"` por Bluetooth clásico.
- Una app Android como **Automate** recibe el mensaje y envía automáticamente un **SMS de emergencia** al contacto configurado.
- El flujo en Automate escucha el mensaje por Bluetooth y ejecuta el envío del SMS sin intervención del usuario.

---

## 💡 Notas

- El backend fue probado en entorno **localhost**, accesible desde TTN gracias a **Ngrok**.
- El sistema puede seguir ampliándose con autenticación avanzada, más sensores, o integración con bases de datos.
- Está diseñado como **prototipo**, pero permite interacción completa y en tiempo real con el operador.

---
