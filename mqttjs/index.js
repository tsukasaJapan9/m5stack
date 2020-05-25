'use strict;'

const http = require('http');
const fs = require('fs');
const url = require('url');
const mqtt = require('mqtt');
const keypress = require('keypress');
const client  = mqtt.connect('mqtt://localhost');
  
process.stdin.setRawMode(true);
process.stdin.resume();

// MQTT
client.subscribe('robot/event');
client.publish('robot/command', 'hello');

client.on('message', function (topic, message) {
    // message is Buffer
    console.log(message.toString());
});

function forward() {
    client.publish('robot/command', '1');
}

function backward() {
    client.publish('robot/command', '2');
}

function right() {
    client.publish('robot/command', '3');
}

function left() {
    client.publish('robot/command', '4');
}

// key入力設定
keypress(process.stdin);

process.stdin.on('keypress', function (ch, key) {
    　//Ctrl+c
    if (key && key.ctrl && key.name == 'c') {
        process.stdin.exit()
    }

    if (key && key.name == 'up') {
        console.log("up")
        forward();
    }
    if (key && key.name == 'down') {
        console.log("down")
        backward();
    }
    if (key && key.name == 'right') {
        console.log("right")
        right();
    }

    if (key && key.name == 'left') {
        console.log("left")
        left();
    }
});


// web server
const server = http.createServer((req, res) => {
    const now = new Date();
    console.info('[' + now + '] Requested by ' + req.connection.remoteAddress + " method: " + req.method);

    res.writeHead(200, {
        'Content-Type': 'text/html; charset=utf-8'
    });

    let url = req.url;
    let tmp = url.split('.');
    let ext = tmp[tmp.length -1]
    let path = '.' + url;

    console.log(url)
    
    switch (req.method) {
        case 'GET':
            if (ext == '/') {
                const rs = fs.createReadStream('./index.html');
                rs.pipe(res);
            } else if (ext == 'js') {
                const rs = fs.createReadStream(path);
                rs.pipe(res);
            }
            break;
        case 'POST':
            console.log('post')
        default:
            break;
    }
}).on('error', e => {
    console.error('[' + new Date() + '] Server error', e)       
}).on('clientError', e => {
    console.error('[' + new Date() + '] Client error', e)       
});

const port = 8000;
server.listen(port, () => {
    console.log('listening on ' + port)
});

// client.end();