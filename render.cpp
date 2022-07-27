#include <Bela.h>
#include <iree/runtime/api.h>

// TODO: read this in via command line
static const char* module_file_path = "/root/module.vmfb";
static iree_runtime_instance_t* instance = NULL;
static iree_runtime_session_t* session = NULL;
static iree_hal_device_t* local_sync_device = NULL;

bool iree_runtime_setup(BelaContext* context, void* userData){
	// get filepath of module(s)
	// setup runtime instance, an instance can run multiple sessions
	iree_runtime_instance_options_t instance_options; // holds API version and driver registry
	iree_runtime_instance_options_initialize(IREE_API_VERSION_LATEST,
											&instance_options);
	
	iree_runtime_instance_options_use_all_available_drivers(&instance_options);

	iree_status_t status = iree_runtime_instance_create(
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

	//setup session
	iree_runtime_session_options_t session_options;
  	iree_runtime_session_options_initialize(&session_options);
	status = iree_runtime_session_create_with_device(
		instance, &session_options, local_sync_device,
		iree_runtime_instance_host_allocator(instance), &session);
	iree_hal_device_release(local_sync_device);

	if (iree_status_is_ok(status)) {
    	status = iree_runtime_session_append_bytecode_module_from_file(session,
                                                               module_file_path);;
  	}

	// initialize module functions
	// allocate input and output buffer
	// setup session
	return true;
}



void iree_runtime_render(BelaContext* context, void* userData){
	volatile int a = 6;
	a += 7;
	// invoke
}

void iree_runtime_cleanup(BelaContext *context, void *userData){
	iree_runtime_instance_release(instance);
	// release runtime instance
}

