#include <nan.h>
#include <stdio.h>

#ifdef LINUX

#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <linux/input.h>

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
        key* ed = NULL;

        int keyboard = open(deviceName.c_str(), O_RDONLY);

        if (keyboard == -1) {
            return;
        }

        ed = new key;
        ed->value = 10;
        callbackEvent.Send((const char*)(ed), sizeof(key));

        while (1) {
            int bytesRead = read(keyboard, callbackEventData, size * 64);

            for (int count = 0; count < (bytesRead / size); count++) {
                if (EV_KEY == callbackEventData[count].type) {
                    ed = new key;
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

#endif

void Init(v8::Handle<v8::Object> exports) {
#ifdef LINUX
    exports->Set(NanNew<v8::String>("obj"), NanNew<v8::FunctionTemplate>(DoProgress)->GetFunction());
#else
    printf("raw-keyboard is not supported on this platform!");
#endif
}

NODE_MODULE(asyncprogressworker, Init)
