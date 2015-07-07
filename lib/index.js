
var worker = require("../build/Release/binding").obj;
var util = require("util");
var EventEmitter = require("events").EventEmitter;

module.exports = function(deviceName) {
    EventEmitter.call(this);

    worker(deviceName, function(code, value) {
        if (value === 1) {
            this.emit("keydown", code);
        } else if (value === 0) {
            this.emit("keyup", code);
        }
    }.bind(this), function() {
        this.emit("error", "Unable to open " + deviceName);
    }.bind(this));
};

util.inherits(module.exports, EventEmitter);
/*
var keyboard = new module.exports("/dev/input/event3");

keyboard.on("keydown", function(code) { console.log("DOWN", code); });
keyboard.on("keyup", function(code) { console.log("UP", code); });
keyboard.on("error", function(msg) { console.log("ERROR", msg); });*/
