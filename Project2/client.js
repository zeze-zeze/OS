'use strict';

const fork = require('child_process').fork;

var cpus = require('os').cpus();
console.log("The number of CPUs: %d", cpus.length);

var connectionNumber = 100;
var testTime = 60000;
var PoWDifficulty = 4;

var counter = 0;
var responseNumber = 0;

for (var i = 0; i < cpus.length; i++) {
    console.log("Fork a new child process ...");

    var args = [];
    args.push(i.toString());
    args.push((connectionNumber/cpus.length).toString());
    args.push(testTime.toString());
    args.push(PoWDifficulty.toString());

    var child = fork('./handler.js', args);
    child.on('message', function(message) {
        counter += 1;
        responseNumber += message;
        if (counter == cpus.length) {
            console.log("Total response number: " + responseNumber);
            console.log("Test time: %d seconds", (testTime/1000));
            console.log("PoW Difficulty: " + PoWDifficulty);
            console.log("Response number per second: " + (responseNumber/(testTime/1000)).toFixed(3));
        }
    });
}
