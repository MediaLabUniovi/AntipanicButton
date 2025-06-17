// Cargar las variables de entorno desde el archivo .env
require('dotenv').config();

const axios = require('axios');

// Leer las credenciales del archivo .env
const CLICK_SEND_USER = process.env.CLICK_SEND_USER;
const CLICK_SEND_API_KEY = process.env.CLICK_SEND_API_KEY;
const DESTINATARIO_SMS = process.env.DESTINATARIO_SMS;

// Función para enviar el SMS
async function sendSMSAlert() {
  const mensaje = '🚨 ¡Botón de pánico activado!';

  try {
    const response = await axios.post(
      'https://rest.clicksend.com/v3/sms/send',
      {
        messages: [
          {
            source: 'nodejs',
            body: mensaje,
            to: DESTINATARIO_SMS,
            from: 'Alerta'
          }
        ]
      },
      {
        auth: {
          username: CLICK_SEND_USER,
          password: CLICK_SEND_API_KEY
        },
        headers: {
          'Content-Type': 'application/json'
        }
      }
    );
    
    console.log('✅ SMS enviado:', mensaje);
    return response.data;  // Retorna la respuesta para verificar si fue exitoso
  } catch (err) {
    console.error('❌ Error al enviar SMS:', err.message);
    throw new Error('No se pudo enviar el SMS');
  }
}

module.exports = sendSMSAlert;



