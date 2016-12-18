var nconf = require('nconf');
nconf.argv()
       .env()
       .file({ file: __dirname + '/config.json' });

var events = require('events');
var eventEmitter = new events.EventEmitter();
var nodeLib = nconf.get('server:nodeLib');
var logfilePath = nconf.get('server:logfilePath');

var bunyan = require('bunyan');

//--------------- Logging middleware ---------------
var log = bunyan.createLogger({
  name: 'eMonitor',
  streams: [
    /*{
      level: 'debug',
      stream: process.stdout            // log INFO and above to stdout
    },*/
    //Log should be outside app folders
    {
      path: logfilePath + 'eMonitor.log'  // log ERROR and above to a file
    }
  ]
});

var fs = require('safefs');
var SEPARATOR = nconf.get('telemetry:SEPARATOR');
var installPath = nconf.get('server:installPath');
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var sys = require('sys');

var interfacePort = nconf.get('server:interfacePort');
var listeningPort = nconf.get('server:listeningPort');
var trasmittingPort = nconf.get('server:trasmittingPort');
var serverPort = nconf.get('server:serverPort');
var version = nconf.get('server:version');

var readSensors = nconf.get('sampling:readSensors');


// include custom functions ======================================================================
//var systemModules = require(installPath + 'server/app/lib/systemModules');
//var functions = require(installPath + 'server/app/lib/functions');

// load the routes
require('./routes.js')(app);

app.use(express.static(installPath + 'server/wwwroot'));

var serverADDR = 'N/A';
var LogR = 0;

/*
 Can use a try... catch statement
 var SerialPort = require("serialport").SerialPort

try {
  var serialPort = new SerialPort("/dev/portdoesntexist");
} catch(error) {
  console.log("Port not ready/doesn't exist!");
}
*/ 


//Get IP address http://stackoverflow.com/questions/3653065/get-local-ip-address-in-node-js

var os = require('os');
var ifaces = os.networkInterfaces();

Object.keys(ifaces).forEach(function (ifname) {
  var alias = 0
    ;

  ifaces[ifname].forEach(function (iface) {
    if ('IPv4' !== iface.family || iface.internal !== false) {
      // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
      return;
    }

    if (alias >= 1) {
      // this single interface has multiple ipv4 addresses
      console.log(ifname + ':' + alias, iface.address);
    } else {
      // this interface has only one ipv4 adress
      serverADDR = iface.address;
    }
  });
});




//---------------

var HOST = '192.168.1.216';
var dgram = require('dgram');

const server = dgram.createSocket('udp4');



server.bind({
  address: '192.168.1.153',
  port: listeningPort
});

server.on('listening', () => {
  var address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  //console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
  //socket.emit('serialData', msg.toString());
eventEmitter.emit('sensorData', msg.toString());

          });

io.on('connection', function(socket){
  //socket.emit('connected', version, Telemetry);

    var myDate = new Date();

   var startMessage = 'Connected ' + myDate.getHours() + ':' + myDate.getMinutes() + ':' + myDate.getSeconds()+ ' v' + version + ' @' + serverADDR;
     socket.emit('connected', startMessage, serverADDR, serverPort);
    console.log('New socket.io connection - id: %s', socket.id);

    eventEmitter.on('sensorData', function(msg){
    socket.emit('serialData', msg);


                  if (msg.indexOf('TEMPERATURE') !== -1)
                  {
                    var tokenData = msg.toString().split(SEPARATOR);
                    var j = 0;
                      //console.log(tokenData[1]);
                    socket.emit('TEMPERATURE', tokenData[1]);

                  }
                  if (msg.indexOf('SOUND') !== -1)
                  {
                    var tokenData = msg.toString().split(SEPARATOR);
                    var j = 0;
                      //console.log(tokenData[1]);
                    socket.emit('SOUND', tokenData[1]);

                  }
                  if (msg.indexOf('GAS') !== -1)
                  {
                    var tokenData = msg.toString().split(SEPARATOR);
                    var j = 0;
                      //console.log(tokenData[1]);
                    socket.emit('GAS', tokenData[1]);

                  }
                  if (msg.indexOf('LIGHT') !== -1)
                  {
                    var tokenData = msg.toString().split(SEPARATOR);
                    var j = 0;
                      //console.log(tokenData[1]);
                    socket.emit('LIGHT', tokenData[1]);

                  }
                  if (msg.indexOf('HUMIDITY') !== -1)
                  {
                    var tokenData = msg.toString().split(SEPARATOR);
                    var j = 0;
                      //console.log(tokenData[1]);
                    socket.emit('HUMIDITY', tokenData[1]);

                  }

  });

    setInterval(function() {
        sendCMD("readSensors", HOST, trasmittingPort);
    }, readSensors);


    function sendCMD(CMD, HOST, PORT) {
        var client = dgram.createSocket('udp4');
        client.bind(10002);

        client.send(CMD, 0, CMD.length, PORT, HOST, function(err, bytes) {
          if (err) throw err;
          //console.log('UDP message ' + CMD +' sent to ' + HOST +':'+ PORT);
          client.close();
        });


        }


  socket.on('REBOOT', function(){
    function puts(error, stdout, stderr) { sys.puts(stdout) }
    log.info('Server rebootiing now');
    exec('sudo reboot now');
    sockets.emit('Info', "Rebooting")

  });

  socket.on('SHUTDOWN', function(){
    socket.emit('Info', "Bailey going down for maintenance now!");
    log.info('Bailey going down for maintenance now!');
    function puts(error, stdout, stderr) { sys.puts(stdout) }
    exec('sudo shutdown now');

  });

  socket.on('disconnect', function(){
    console.log('Disconnected id: %s', socket.id);
    log.info('Client disconnected ' + socket.id);
  });

  socket.on('connected', function(){
    //console.log('CONNECTED id: %s', socket.id);
   // log.info('Client disconnected ' + socket.id);sudo modprobe bcm2835-v4l2
  });


    eventEmitter.on('CMDecho', function(data){
        socket.emit('CMD', data);

  });

    eventEmitter.on('serialData', function(data){
        socket.emit('serialData', data);

  });

});

io.on('disconnect', function () {
        console.log('A socket with sessionID ' + hs.sessionID
            + ' disconnected!');
    log.info('A socket with sessionID ' + hs.sessionID
            + ' disconnected!');
    });

http.listen(serverPort, function(){
console.log('listening on *: ' + serverADDR + ':' + serverPort);

    });



module.exports.nconf = nconf;
    
  
