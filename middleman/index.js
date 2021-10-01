const SP = require('serialport');
const port = new SP('/dev/ttyACM0', { baudRate: 9600 });

port.on('readable', ()=>{
    console.log(port.read().toString());
});
