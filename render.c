#include <Bela.h>
#include <iree/runtime/api.h>
#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/modules/hal/module.h"
#include "iree/vm/api.h"
#include "iree/vm/bytecode_module.h"

#define M_PI 3.14159

// TODO: read this in via command line
char* module_file_path = "/root/module.vmfb";
iree_runtime_instance_t* instance = NULL;
iree_runtime_session_t* session = NULL;
iree_hal_device_t* local_sync_device = NULL;
iree_runtime_call_t module_call;
iree_hal_buffer_view_t* input_hal_buffer_view[2];
iree_hal_buffer_view_t* output_hal_buffer_view = NULL;

// TODO: set this based on the input size of the model automatically?
const uint32_t block_size = 1024;
const iree_hal_dim_t shape[2] = {1024, 1};

float gFrequency = 440.0;
float gPhase;
float gInverseSampleRate;

AuxiliaryTask gIREETask;
void bela_iree_invoke();



bool iree_runtime_setup(BelaContext* context, void* userData){
	iree_status_t status;
	// get filepath of module(s)
	// setup runtime instance, an instance can run multiple sessions
	iree_runtime_instance_options_t instance_options; // holds API version and driver registry
	iree_runtime_instance_options_initialize(IREE_API_VERSION_LATEST,
											&instance_options);
	
	iree_runtime_instance_options_use_all_available_drivers(&instance_options);

	status = iree_runtime_instance_create(
		&instance_options, iree_allocator_system(), &instance
	);

	if(!iree_status_is_ok(status)){
		iree_status_fprint(stderr, status);
		return false; // failed to create instance
	}

	//get local-sync device
	IREE_RETURN_IF_ERROR(iree_runtime_instance_try_create_default_device(
		instance, iree_make_cstring_view("local-sync"), &local_sync_device
	));

	if(!iree_status_is_ok(status)){
		iree_status_fprint(stderr, status);
		return false; // failed to create device
	}

	//setup session
	iree_runtime_session_options_t session_options;
  	iree_runtime_session_options_initialize(&session_options);
	status = iree_runtime_session_create_with_device(
		instance, &session_options, local_sync_device,
		iree_runtime_instance_host_allocator(instance), &session);
	iree_hal_device_release(local_sync_device);

	status = iree_runtime_session_append_bytecode_module_from_file(session,
                                                               module_file_path);;
	if(!iree_status_is_ok(status)){
		iree_status_fprint(stderr, status);
		return false; // failed to load bytecode module
	}
	iree_runtime_call_initialize_by_name(
		session, iree_make_cstring_view("module.main"), &module_call);

	if(!iree_status_is_ok(status)){
		iree_status_fprint(stderr, status);
		return false; // failed to initialize module function
	}

	iree_hal_allocator_t* device_allocator =
		iree_runtime_session_device_allocator(session);
  	iree_allocator_t host_allocator =
		iree_runtime_session_host_allocator(session);
  	status = iree_ok_status();

	IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
		iree_hal_device_allocator(local_sync_device), IREE_ARRAYSIZE(shape), shape,
		IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
		(iree_hal_buffer_params_t){
			.type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
			.usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
		},
		/* initial data */ iree_make_const_byte_span(NULL, 1024), &input_hal_buffer_view[0]));

	IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
		iree_hal_device_allocator(local_sync_device), IREE_ARRAYSIZE(shape), shape,
		IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
		(iree_hal_buffer_params_t){
			.type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
			.usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
		},
		/* initial data */ iree_make_const_byte_span(NULL, 1024), &input_hal_buffer_view[1]));
	// ensure output buffer is zeroed out to avoid clicks on startup

	status = iree_runtime_call_inputs_push_back_buffer_view(
		&module_call, input_hal_buffer_view[0]);
		

	if((gIREETask = Bela_createAuxiliaryTask(&bela_iree_invoke, 90, "iree", NULL)) == 0)
		return false;
	
	// run the model
	status = iree_runtime_call_invoke(&module_call, /*flags*/0);
	if(!iree_status_is_ok(status)){
		iree_status_fprint(stderr, status);
		return false;
	}
	
  if (iree_status_is_ok(status)) {
    // This prints the buffer view out but an application could read its
    // contents, pass it to another call, etc.
    //status = iree_hal_buffer_view_fprint(
      //  stdout, ret0, /*max_element_count=*/4096, host_allocator);
  }

	fprintf(stdout, "%d\n", iree_vm_list_size(module_call.outputs));
	iree_runtime_call_outputs_pop_front_buffer_view(&module_call, &output_hal_buffer_view);
	// read the output
	// how to get output buffer??


    fprintf(stdout, "\n * \n");

	gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0.0;

	return true;
}

void test_sine(BelaContext* context){
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = 0.8f * sinf(gPhase);
		gPhase += 2.0f * (float)M_PI * gFrequency * gInverseSampleRate;
		if(gPhase > M_PI)
			gPhase -= 2.0f * (float)M_PI;

		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
	}
}



void iree_runtime_render(BelaContext* context, void* userData){
	static bool inputBufferToggle = false; // denotes which buffer is being processed and which is being filled
	static uint32_t bufferByteWriteCount = 0;
	iree_status_t status;
	test_sine(context);

	// once last call is done, toggle, swap buffers and invoke again
	if((iree_vm_list_size(module_call.outputs) > 0) && (bufferByteWriteCount >= block_size)){ // if output is ready and previous buffer is all read out
		output_hal_buffer_view = NULL;
		status = iree_runtime_call_outputs_pop_front_buffer_view(&module_call, &output_hal_buffer_view);
		iree_runtime_call_reset(&module_call);
		iree_runtime_call_inputs_push_back_buffer_view(&module_call, input_hal_buffer_view[inputBufferToggle]);
		inputBufferToggle = ~inputBufferToggle;
		bufferByteWriteCount = 0;
		Bela_scheduleAuxiliaryTask(gIREETask);
		//rt_printf("toggle.\n");
	}
	else if(bufferByteWriteCount >= block_size){
		return false; // model missed latency deadline
	}

	// write data into preparation hal buffer for IREE
	uint32_t dataTransferSize = min(block_size - bufferByteWriteCount, context->audioFrames);
	status = iree_hal_buffer_map_write(iree_hal_buffer_view_buffer(input_hal_buffer_view[inputBufferToggle]),
		bufferByteWriteCount, context->audioIn, dataTransferSize);
	// write data out from previous invocation
	status = iree_hal_buffer_map_read(iree_hal_buffer_view_buffer(output_hal_buffer_view),
		bufferByteWriteCount, context->audioOut, dataTransferSize);
	bufferByteWriteCount += dataTransferSize;


}

void bela_iree_invoke(){
	iree_runtime_call_invoke(&module_call, /*flags*/ 0);
}

void iree_runtime_cleanup(BelaContext *context, void *userData){
	iree_runtime_instance_release(instance);
	iree_runtime_session_release(session);
	// TODO: release local-sync device?
}

