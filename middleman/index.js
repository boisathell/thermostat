const SP = require('serialport');
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const BOARD_FILTER_WORD = "Seeeduino"

async function main() {

  const { stdout, stderr } = await exec(
    "/home/devil/bin/arduino-cli board list | grep " + 
    BOARD_FILTER_WORD + 
    " | awk '{print $1}'"
  );
  if (stderr) { throw stderr; }

  let port = stdout.split('\n')[0];
  if (!port.startsWith('/dev/')) { throw new Error('Port for "' + BOARD_FILTER_WORD + '" not found.'); }


  const seeed = new SP(port, { baudRate: 9600 });

  seeed.on('readable', ()=>{
    let data = seeed.read().toString().split('\n');
    let jstr = " " + (new Date()).getTime().toString() + "\n";
    let out = data.join(jstr);
    process.stdout.write(out);
  });

}

main();
