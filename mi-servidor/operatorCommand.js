const sendDownlinkToTTN = require('./sendDownlinkToTTN');  // Importamos la función que envía el downlink
const CancelDownlink = require('./CancelDownlink');  // Importamos la función que cancela el downlink

// Función que procesa el comando del operador
function processOperatorCommand(command) {
    let payload;

    // Verifica el comando recibido y convierte a Base64
    switch (command) {
        case "02":
            payload = Buffer.from([0x02]).toString('base64');  // Convierte el valor 0x02 a Base64

	// Llama a la función sendDownlinkToTTN con el payload en Base64
    	sendDownlinkToTTN(payload);  // Asegúrate de que la función sendDownlinkToTTN esté lista para recibir este payload.

            break;

	case "03":
            payload = Buffer.from([0x03]).toString('base64');  // Convierte el valor 0x03 a Base64

	// Llama a la función sendDownlinkToTTN con el payload en Base64
    	sendDownlinkToTTN(payload);  // Asegúrate de que la función sendDownlinkToTTN esté lista para recibir este payload.

            break;
        // Puedes agregar más casos según sea necesario
        default:
            console.log(`Comando no reconocido: ${command}`);
            return;  // Si el comando no es reconocido, no hace nada
    }

    
}

module.exports = processOperatorCommand;
