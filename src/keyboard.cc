#include <nan.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

struct key {
    __u16 code;
    __s32 value;
};

class KeyboardWorker: public NanAsyncProgressWorker {
public:
    KeyboardWorker(NanCallback *callbackComplete, NanCallback *callbackEvent, std::string deviceName): NanAsyncProgressWorker(callbackComplete), callbackEvent(callbackEvent), deviceName(deviceName) {}
    ~KeyboardWorker() {}

    void Execute(const NanAsyncProgressWorker::ExecutionProgress& callbackEvent) {
        input_event callbackEventData[64];
        int size = sizeof(input_event);

        int keyboard = open(deviceName.c_str(), O_RDONLY);

        if (keyboard == -1) {
            return;
        }

        while (1) {
            int bytesRead = read(keyboard, callbackEventData, size * 64);

            for (int count = 0; count < (bytesRead / size); count++) {
                if (EV_KEY == callbackEventData[count].type) {
                    key* ed = new key;
                    ed->code = callbackEventData[count].code;
                    ed->value = callbackEventData[count].value;

                    callbackEvent.Send((const char*)(ed), sizeof(key));
                }
            }
        }
    }

    void HandleProgressCallback(const char *data, size_t size) {
        NanScope();

        key* ed = (key*)data;

        v8::Local<v8::Value> argv[] = {
            NanNew<v8::Integer>(ed->code),
            NanNew<v8::Integer>(ed->value)
        };

        callbackEvent->Call(2, argv);
    }

private:
    NanCallback *callbackEvent;
    std::string deviceName;
};

NAN_METHOD(DoProgress) {
    NanScope();

    NanCallback *callbackEvent = new NanCallback(args[1].As<v8::Function>());
    NanCallback *callbackComplete = new NanCallback(args[2].As<v8::Function>());
    v8::String::Utf8Value str(args[0]->ToString());

    NanAsyncQueueWorker(new KeyboardWorker(callbackComplete, callbackEvent, *str));

    NanReturnUndefined();
}

void Init(v8::Handle<v8::Object> exports) {
    exports->Set(NanNew<v8::String>("obj"), NanNew<v8::FunctionTemplate>(DoProgress)->GetFunction());
}

NODE_MODULE(asyncprogressworker, Init)
