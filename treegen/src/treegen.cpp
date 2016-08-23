#include <node/node.h>
#include <node/v8.h>

#include "tree.hpp"

using v8::FunctionCallbackInfo;
using v8::Value;
using v8::Isolate;
using v8::HandleScope;
using v8::Handle;

using tree::TreeConfigs;
using tree::Tree;

void HelloWorld(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "world"));
}

void Generate(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    TreeConfigs treeData;

    if( args[0]->IsObject()) {

        Handle<v8::Object> config = Handle<v8::Object>::Cast(args[0]);

        treeData.baseLength = Handle<v8::Value>::Cast(config->Get(
            v8::String::NewFromUtf8(isolate, "baseLength")
        ))->NumberValue();
        treeData.widthHeightRatio = Handle<v8::Value>::Cast(config->Get(
            v8::String::NewFromUtf8(isolate, "widthHeightRatio")
        ))->NumberValue();
        treeData.maxEnergy = Handle<v8::Value>::Cast(config->Get(
            v8::String::NewFromUtf8(isolate, "maxEnergy")
        ))->NumberValue();
        treeData.branchEnergyRatio = Handle<v8::Value>::Cast(config->Get(
            v8::String::NewFromUtf8(isolate, "branchEnergyRatio")
        ))->NumberValue();

        auto tree = Tree::generate( 1, treeData );

        v8::Local<v8::Object> obj = v8::Object::New(isolate);
        obj->Set(
            v8::String::NewFromUtf8(isolate, "nodeCount"),
            v8::Number::New(isolate, tree->countNodes() )
        );
        args.GetReturnValue().Set(obj);
        return;
    }

    args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
}


void init(v8::Local<v8::Object> target) {
    NODE_SET_METHOD(target, "hello", HelloWorld);
    NODE_SET_METHOD(target, "generate", Generate);
}

NODE_MODULE(binding, init);


/*

#include <chrono>
#include <thread>

#include <ctime>

int main( int argc, char* argv[] ) {

	int treeCount = 100;
	int progressSkip = treeCount / 19;
	size_t waitMilliseconds = 8;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for(int t = 0; t < treeCount; t++ ) {
		TreeConfigs treeData;

		treeData.baseLength = 1.0f;
		treeData.widthHeightRatio = 0.3f;
		treeData.maxEnergy = 10.0f;
		treeData.branchEnergyRatio = 0.5f;

		auto tree = Tree::generate( 1, treeData );

		//printf("Tree generated with %zu nodes\n", tree->countNodes() );
		if( t % progressSkip == 0 ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(waitMilliseconds));
			printf("%d...", t);
		}
	}
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;
	printf("\nTook %f seconds to generate %d trees\n", elapsed_seconds.count(), treeCount );

	return 0;
}


## Todo

*


## Conventions

* State knows how to format itself
* Recusive functions end in `_r`
* Accessors that return state must start with `get`
* Accessors that return computed values must be named such that it isn't just
"get this", instead it should be "compute this" or "count these"
* Classes that use Handles should have static factory functions
* Classes that use Handles should have a field called `self` that the factory sets
* Classes that use Handles should have private constructors
* All functions that return more than 64 bits are deprecated
* Use doxygen to generate the documentation for the code
* If a function needs to return a large object,
	* It must the first parameter in the function
	* It must be passed by a reference to a pointer
	* It must return a bool (success is `true`)
	* The base class must be Object
	* If the function returns `false`, it must add at least one error


*/
