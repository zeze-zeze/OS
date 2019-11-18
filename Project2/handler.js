'use strict';

const net = require('net');
const sha256 = require('js-sha256');

function generateRandomString() {
    var str = "";
    var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (var i = 0; i < 5; i++) {
        str += possible.charAt(Math.floor(Math.random() * possible.length));
    }

    return str;
}

function generateRandomInt() {
    return Math.floor(Math.random() * 10);
}

function checkPoW(message, seed, hash, difficulty) {
    var s = seed + message;
    if (sha256(s) != hash) {
        return false;
    }

    for (var i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            console.log(i);
            return false;
        }
    }
    return true;
}

var id = process.argv[2];
var connectionNumber = process.argv[3];
var testTime = process.argv[4];
var PoWDifficulty = process.argv[5];

var responseNumber = 0;

for (var i = 0; i < connectionNumber; i++) {
    var socket = new net.Socket();

    socket.connect(20000, '127.0.0.1', function() {
        var message = generateRandomString();
        console.log("[%d] Send to server: %s", id, message);
        this.write(message + '\n');
    });

    socket.on('data', function(data) {
        var socketRef = this;
        
        data = data.toString();
        data = data.substring(0, data.length-1);
        data = data.split('\n');
        data.forEach(function(element) {
            console.log("[%d] Received from server: %s", id, element);

            var dataList = element.split(',');
            var message = dataList[0];
            var seed = dataList[1];
            var hash = dataList[2];

            if (generateRandomInt() == 0) {
                console.log("[%d] check whether response data is valid", id);
                console.log(sha256(seed+message));
                if (checkPoW(message, seed, hash, PoWDifficulty) == false) {
                    console.log("[%d] Invalid response data", id);
                    console.log("[%d] message: %s", id, message);
                    console.log("[%d] seed: %s", id, seed);
                    console.log("[%d] hash: %s", id, hash);
                    console.log(seed+message)
                    process.exit(1);
                };
            }
            responseNumber += 1;

            message = generateRandomString();
            console.log("[%d] Send to server: %s", id, message);
            socketRef.write(message + '\n');
        }); 
    });
}

process.on('uncaughtException', function(err) {
    return;
});

setTimeout(function() {
    process.send(responseNumber);
    process.exit(0);
}, testTime);
