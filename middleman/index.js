const SP = require('serialport');
const seeed = new SP('/dev/ttyACM0', { baudRate: 9600 });
const mega = new SP('/dev/ttyACM1', { baudRate: 9600 }); 

let write_t = 0;
mega.on('readable', ()=>{
   let t = mega.read().toString();
   //process.stdout.write(t);
   write_t = (new Date()).getTime();
   //seeed.write('ABC\n');
   seeed.write(t);
});

seeed.on('readable', ()=>{
   //console.log((new Date()).getTime() - write_t); 
  // let data = seeed.read().toString() + " " + (new Date()).getTime().toString();
   
  let data = seeed.read().toString().split('\n');
  let jstr = " " + (new Date()).getTime().toString() + "\n";
  let out = data.join(jstr);
  process.stdout.write(out);
  
});
