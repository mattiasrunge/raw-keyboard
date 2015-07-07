# raw-keyboard
Capture raw input in Linux

## Example
```javascript
var keyboard = new module.exports();

keyboard.on("keydown", function(code) { console.log("DOWN", code); });
keyboard.on("keyup", function(code) { console.log("UP", code); });
keyboard.on("error", function(msg) { console.log("ERROR", msg); });
keyboard.on("ready", function() { console.log("READY"); });
keyboard.open("/dev/input/event3");
```
