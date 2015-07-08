
var worker = require("../build/Release/binding").obj;
var util = require("util");
var EventEmitter = require("events").EventEmitter;
var codes = require("./codes.json");
// cat /usr/include/linux/input.h | grep define | grep KEY | sed -e 's/\t\+/ /g' | cut -d' ' -f2,3 | awk '{print $2,$1}' | sed -e 's/ /\": \"/g' | sed -e 's/$/\",/g' | sed -e 's/^/    \"/g' > lib/codes.json

module.exports = function() {
    EventEmitter.call(this);

    this.open = function(deviceName, keys) {
        keys = keys.map(function(key) {
            for (var code in Object.keys(codes)) {
                if (codes[code] === key) {
                    return code;
                }
            }

            return null;
        });

        worker(deviceName, function(code, value) {
            console.log(code, value);
            if (value === 10) {
                return this.emit("ready");
            }

            code = code.toString();

            if (keys.indexOf(code) === -1) {
                return;
            }

            if (value === 1) {
                this.emit("keydown", codes[code]);
            } else if (value === 0) {
                this.emit("keyup", codes[code]);
            }
        }.bind(this), function() {
            return this.emit("error", "Unable to open " + deviceName);
        }.bind(this));
    };
};

util.inherits(module.exports, EventEmitter);
/*
var keyboard = new module.exports();

keyboard.on("keydown", function(code) { console.log("DOWN", code); });
keyboard.on("keyup", function(code) { console.log("UP", code); });
keyboard.on("error", function(msg) { console.log("ERROR", msg); });
keyboard.on("ready", function() { console.log("READY"); });
keyboard.open("/dev/input/event32", [ "KEY_P" ]);*/
