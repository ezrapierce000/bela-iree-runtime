#include <Bela.h>
#include <iree/runtime/api.h>

bool iree_runtime_setup(BelaContext* context, void* userData){
	// get filepath of module(s)
	// setup runtime instance
	// initialize module functions
	// allocate input and output buffer
	// setup session



}



void iree_runtime_render(BelaContext* context, void* userData){
	volatile int a = 6;
	a += 7;
	// invoke
}

void iree_runtime_cleanup(BelaContext *context, void *userData){

	// release runtime instance
}

