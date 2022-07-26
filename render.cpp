#include <Bela.h>
#include <iree/runtime/api.h>

bool iree_runtime_setup(BelaContext* context, void* userData){
	return true;
}


void iree_runtime_render(BelaContext* context, void* userData){
	volatile int a = 6;
	a += 7;
}

void iree_runtime_cleanup(BelaContext *context, void *userData){
	int b = 9;
}
