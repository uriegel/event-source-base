#pragma once
#include <napi.h>
#include <uv.h>

class EventSourceBase {
public:
    EventSourceBase(const Napi::Function& callback) 
    : callback(Persistent(callback))
    {}
    virtual ~EventSourceBase() {
        callback.Reset();
    }
    void SendEvent() { uv_async_send(&uv_async); }    
    void Dispose() {
        // This is a cancellation of an action registered on V8 thread.
        // Unref the handle to stop preventing the process from exiting.
        uv_unref(reinterpret_cast<uv_handle_t*>(&uv_async));
        uv_close(reinterpret_cast<uv_handle_t*>(&uv_async), CloseCallback);
    }
protected:
    void Initialize() {
        uv_async_init(
            uv_default_loop(), 
            &uv_async, 
            reinterpret_cast<uv_async_cb>(AsyncExecuteComplete)
        );
        uv_async.data = this;
    }
    virtual void OnEvent() = 0;
private:
    static void AsyncExecuteComplete(uv_async_t* handle, int status) {
        auto esb = reinterpret_cast<EventSourceBase*>(handle->data);
        // This executes on V8 thread
        esb->OnEvent();
    }
public:
    static void CloseCallback(uv_handle_t* handle) {
        auto uv_async = reinterpret_cast<uv_async_t*>(handle);
        auto esb = reinterpret_cast<EventSourceBase*>(uv_async->data);
        delete esb;
    }
protected:
    Napi::FunctionReference callback;
private:    
    uv_async_t uv_async{0};
};