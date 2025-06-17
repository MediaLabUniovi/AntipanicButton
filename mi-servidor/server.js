const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
require('dotenv').config();

const sendDownlinkToTTN = require('./sendDownlinkToTTN');
const sendWhatsAppAlert = require('./sendWhatsAppAlert');
const sendSMSAlert = require('./sendSMSAlert');
const processOperatorCommand = require('./operatorCommand');

const app = express();
const port = 3000;

app.use(bodyParser.json());
app.use(cors());
app.use(express.static('public'));

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/public/index.html');
});

app.get('/mapa', (req, res) => {
    res.sendFile(__dirname + '/public/mapa.html');
});

// WebSocket
const WebSocket = require('ws');
const wss = new WebSocket.Server({ noServer: true });
let gpsSent = false;

wss.on('connection', (ws) => {
    console.log('Cliente WebSocket conectado');
    ws.on('close', () => {
        console.log('Cliente WebSocket desconectado');
    });
});

// Webhook de TTN
app.post('/webhooks', async (req, res) => {
    const uplinkMessage = req.body;
    console.log('Uplink recibido');

    if (uplinkMessage && uplinkMessage.uplink_message) {
        const frmPayload = uplinkMessage.uplink_message.frm_payload;

        if (frmPayload) {
            console.log('Payload recibido:', frmPayload);
            const buffer = Buffer.from(frmPayload, 'base64');

             //Enviar WhatsApp
            sendWhatsAppAlert();

            //Enviar SMS
             try {
                await sendSMSAlert();
            } catch (error) {
                console.error('Error al enviar SMS:', error);
            }

            if (!gpsSent && buffer.length >= 9) {
                const lat = buffer.readFloatLE(1);
                const lon = buffer.readFloatLE(5);

                console.log(`Latitud: ${lat}, Longitud: ${lon}`);

                if (!isNaN(lat) && !isNaN(lon) && lat !== 0.0 && lon !== 0.0) {
                    
                    //Enviar Coordenadas GPS
                    const gpsData = { lat, lon, time: new Date().toISOString() };

                    wss.clients.forEach(client => {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send(JSON.stringify(gpsData));
                        }
                    });

                    gpsSent = true;
                    console.log("📍 Coordenadas enviadas una vez al mapa.");


                    console.log('Esperando a que el operador reaccione...');

                } else {
                    console.log('Coordenadas inválidas (0.0, 0.0), no enviadas al mapa.');
                }
            } else if (buffer.length < 9) {
                console.warn('❌ Buffer demasiado corto para extraer coordenadas:', buffer);
            }

            /*if (uplinkMessage.uplink_message.f_port === 1 && frmPayload === 'AQ==') {
                sendWhatsAppAlert();
                try {
                    await sendSMSAlert();
                } catch (error) {
                    console.error('Error al enviar SMS:', error);
                }
                console.log('Esperando a que el operador reaccione...');
            }*/

            if (frmPayload === 'BA==') {
                console.log('Payload es 0x04. Enviando downlink con payload 0x02...');
                sendDownlinkToTTN("Ag==");

                console.log('Payload es 0x03. Enviando downlink con payload 0x03...');
                sendDownlinkToTTN("Aw==");
            }
        } else {
            console.log('Error: frm_payload no encontrado o vacío');
        }
    } else {
        console.log('Error: uplink_message no está presente');
    }

    res.status(200).send('OK');
});

// Comandos del operador
app.post('/operator-command', (req, res) => {
    const { command } = req.body;

    console.log('Comando recibido del operador:', command);

    if (command) {
        try {
            processOperatorCommand(command);
            res.status(200).send('Comando procesado exitosamente');
        } catch (error) {
            console.error('Error al procesar el comando:', error);
            res.status(500).send('Error interno del servidor');
        }
    } else {
        res.status(400).send('Comando no recibido o inválido');
    }
});


// Iniciar servidor
const server = app.listen(port, () => {
    console.log(`Servidor corriendo en http://localhost:${port}`);
});

// Asociar WebSocket con el mismo servidor HTTP
server.on('upgrade', (req, socket, head) => {
    wss.handleUpgrade(req, socket, head, (ws) => {
        wss.emit('connection', ws, req);
    });
});
