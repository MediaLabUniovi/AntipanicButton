const axios = require('axios');

// Función para cancelar el downlink a TTN 
function CancelDownlink(payload) { 
    const downlinkPayload = {
        downlinks: [
            {
                frm_payload: payload,  // Usamos el payload recibido como argumento
                f_port: 1,             // Puerto de la aplicación (debe ser 1 si ese es el puerto que usas)
                priority: "NORMAL"     // Prioridad del downlink
            }
        ]
    };

    // La URL de la API de TTN para enviar downlinks (reemplaza los valores con tu información)
    const url = `https://eu1.cloud.thethings.network/api/v3/as/applications/boton-del-panico/webhooks/panic-button/devices/panic-button/down/replace`;

    // API Key de TTN (reemplaza con tu API Key)
    const apiKey = 'Bearer NNSXS.S7NLX6NYXL6WSDOGIE2AUTMXZ2MSHGH4AQZPKMQ.QF5LE5D32KSVVTHAYEQ4H6KSCG4UU4SQIZSXN2TRJVZHZNH5KF4A';

    // Hacer la solicitud POST a TTN para enviar el downlink
    axios.post(url, downlinkPayload, {
        headers: {
            'Authorization': apiKey,
            'Content-Type': 'application/json'
        }
    })
    .then(response => {
        console.log('Downlink enviado:', response.data);
    })
    .catch(error => {
        console.error('Error al enviar downlink:', error.response ? error.response.data : error.message);
    });
}

module.exports = CancelDownlink;