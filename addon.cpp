#include <napi.h>
#include <thread>
#include "event-source-base.h"
using namespace Napi;
using namespace EventSourceBase;
using namespace std;

class EventSource : public EventSourceBase {
public:
    EventSource(const Napi::Function& callback) 
    : EventSourceBase(callback)
    {}
    void OnEvent() override {
        HandleScope scope(callback.Env());
        vector<napi_value> args;
        args.push_back(Number::New(callback.Env(), state));
        callback.Call(args);
    }
    void Start() {
        Initialize();
        thread thread([this] {
            Sleep(2000);
            while (isRunning)
            {
                state++;
                SendEvent();
                Sleep(2000);
            }
        });
        thread.detach();        
    }
    void Stop() {
        Dispose();
        isRunning = false;
    }
private:
    int state{0};
    bool isRunning{true};
};

Value RegisterTestEvents(const CallbackInfo& info) {
    auto eventSource = new EventSource(info[0].As<Function>());
    eventSource->Start();
    return Number::New(info.Env(), static_cast<double>(reinterpret_cast<uintptr_t>(eventSource)));
}

Value UnregisterTestEvents(const CallbackInfo& info) {
    EventSource* es = reinterpret_cast<EventSource*>(static_cast<uintptr_t>(info[0].As<Number>().DoubleValue()));
    es->Stop();
    return info.Env().Undefined();
}

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "registerTestEvents"), Function::New(env, RegisterTestEvents));
    exports.Set(String::New(env, "unregisterTestEvents"), Function::New(env, UnregisterTestEvents));
    return exports;
}

const char* extension = "event-source-base-test";
NODE_API_MODULE(extension, Init)