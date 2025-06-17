const twilio = require('twilio');
require('dotenv').config();

// Configura Twilio para enviar mensajes de WhatsApp
const client = new twilio(
    process.env.TWILIO_ACCOUNT_SID,   // Tu Account SID de Twilio
    process.env.TWILIO_AUTH_TOKEN     // Tu Auth Token de Twilio
);

// Función para enviar un mensaje de WhatsApp
function sendWhatsAppAlert() {
    const message = `🚨 *¡Botón de pánico presionado!* 🚨\nSe ha activado una alerta. Por favor, revisa la situación.`

   // Elimina cualquier espacio extra de los números en el .env
    const fromNumber = process.env.TWILIO_WHATSAPP_NUMBER.trim(); 
    const toNumber = process.env.MI_NUMERO_WHATSAPP.trim();

    // Asegúrate de que los números estén bien formateados
    //console.log("Enviando desde:", fromNumber);
    //console.log("Enviando a:", toNumber);

    client.messages.create({
        from: fromNumber,  // Número de WhatsApp de Twilio
        to: toNumber,      // Tu número de WhatsApp
        body: message
    })
    .then(message => {
        console.log('✅ Alerta de WhatsApp enviada:', message.sid);
    })
    .catch(error => {
        console.error('❌ Error al enviar WhatsApp:', error.message);
    });
}

module.exports = sendWhatsAppAlert;
