#define NAPI_EXPERIMENTAL
#include <napi.h>
using namespace Napi;

Object Init(Env env, Object exports) {
    //exports.Set(String::New(env, "test"), Function::New(env, Test));
    return exports;
}

const char* extension = "event-source-base-test";
NODE_API_MODULE(extension, Init)