#include <Bela.h>
#include <iree/runtime/api.h>

// TODO: read this in via command line
static const char* module_file_path = "/root/module.vmfb";
static iree_runtime_instance_t* instance = NULL;
static iree_runtime_session_t* session = NULL;
static iree_hal_device_t* local_sync_device = NULL;
static iree_vm_function_t* function = NULL;



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
/*
	iree_runtime_call_t call;

	IREE_RETURN_IF_ERROR(iree_runtime_call_initialize_by_name(
		session, iree_make_cstring_view("module.main"), &call));
*/

	iree_hal_allocator_t* device_allocator =
		iree_runtime_session_device_allocator(session);
  	iree_allocator_t host_allocator =
		iree_runtime_session_host_allocator(session);
  	status = iree_ok_status();

/*
    // %arg0: tensor<4xf32>
    iree_hal_buffer_view_t* arg0 = NULL;
    if (iree_status_is_ok(status)) {
      static const iree_hal_dim_t arg0_shape[2] = {1024, 1};
      static const float arg0_data[1024] = {1.0f, 1.1f, 1.2f, 1.3f};
      status = iree_hal_buffer_view_allocate_buffer(
          device_allocator,
          // Shape rank and dimensions:
          IREE_ARRAYSIZE(arg0_shape), arg0_shape,
          // Element type:
          IREE_HAL_ELEMENT_TYPE_FLOAT_32,
          // Encoding type:
          IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
          (iree_hal_buffer_params_t){
              // Where to allocate (host or device):
              .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
              // Access to allow to this memory (this is .rodata so READ only):
              .access = IREE_HAL_MEMORY_ACCESS_READ,
              // Intended usage of the buffer (transfers, dispatches, etc):
              .usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
          },
          // The actual heap buffer to wrap or clone and its allocator:
          iree_make_const_byte_span(arg0_data, sizeof(arg0_data)),
          // Buffer view + storage are returned and owned by the caller:
          &arg0);
    }

    if (iree_status_is_ok(status)) {
      IREE_IGNORE_ERROR(iree_hal_buffer_view_fprint(
          stdout, arg0, 4096, host_allocator));
      // Add to the call inputs list (which retains the buffer view).
      status = iree_runtime_call_inputs_push_back_buffer_view(&call, arg0);
    }
    // Since the call retains the buffer view we can release it here.
    iree_hal_buffer_view_release(arg0);
*/

/* lookup fucntion by name, save pointer to function, then call runtime_session_call from render

	iree_runtime_session_lookup_function(
    const iree_runtime_session_t* session, iree_string_view_t full_name,
    iree_vm_function_t* out_function);
*/
	//iree_runtime_call_invoke(&call, /*flags=*/0);
	//iree_rutime_session_call(session, )
	// Dump the function outputs.

	iree_vm_function_t other;
	iree_runtime_session_lookup_function(session, iree_make_cstring_view("module.main"),
    &other);
	

    fprintf(stdout, "\n * \n");

	return true;
}



void iree_runtime_render(BelaContext* context, void* userData){
	static iree_vm_function_t other;
	static bool lookup = false;
	if(!lookup){
		iree_runtime_session_lookup_function(session, iree_make_cstring_view("module.main"),
		&other);
		lookup = true;
	}


	// Synchronously issues a generic function call.
//
// |input_list| is used to pass values and objects into the target function and
// must match the signature defined by the compiled function. List ownership
// remains with the caller.
//
// |output_list| is populated after the function completes execution with the
// output values and objects of the function. List ownership remains with the
// caller.
//
// Functions with either no inputs or outputs may provide NULL for the
// respective list.
/*
IREE_API_EXPORT iree_status_t iree_runtime_session_call(
    iree_runtime_session_t* session, const iree_vm_function_t* function,
    iree_vm_list_t* input_list, iree_vm_list_t* output_list);
	*/
	iree_runtime_session_call(session, &other, NULL, NULL);
}

void iree_runtime_cleanup(BelaContext *context, void *userData){
	iree_runtime_instance_release(instance);
	// release runtime instance
}

