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
            this_thread::sleep_for(2s);
            while (isRunning)
            {
                state++;
                SendEvent();
                this_thread::sleep_for(2s);
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

String GetInclude(const Env& env) {
    napi_value result;
    napi_run_script(env, String::New(env, "process.cwd() + '/node_modules/event-source-base'"), &result);
    return String(env, result);
}

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "registerTestEvents"), Function::New(env, RegisterTestEvents));
    exports.Set(String::New(env, "unregisterTestEvents"), Function::New(env, UnregisterTestEvents));
    exports.Set(String::New(env, "include"), GetInclude(env));
    return exports;
}

const char* extension = "event-source-base-test";
NODE_API_MODULE(extension, Init)