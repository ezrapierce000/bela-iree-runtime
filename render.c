#include <Bela.h>
#include <iree/runtime/api.h>
#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/modules/hal/module.h"
#include "iree/vm/api.h"
#include "iree/vm/bytecode_module.h"

// TODO: read this in via command line
char* module_file_path = "/root/module.vmfb";
iree_runtime_instance_t* instance = NULL;
iree_runtime_session_t* session = NULL;
iree_hal_device_t* local_sync_device = NULL;
iree_hal_buffer_view_t* input_hal_buffer_view;
iree_hal_buffer_view_t* output_hal_buffer_view;
iree_hal_buffer_t* input_buffer;
iree_runtime_call_t module_call;
iree_hal_dim_t shape[2] = {1024, 1};



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
	//iree_hal_device_release(local_sync_device);

	if (iree_status_is_ok(status)) {
    	status = iree_runtime_session_append_bytecode_module_from_file(session,
                                                               module_file_path);;
  	}
	IREE_RETURN_IF_ERROR(iree_runtime_call_initialize_by_name(
		session, iree_make_cstring_view("module.main"), &module_call));

	iree_hal_allocator_t* device_allocator =
		iree_runtime_session_device_allocator(session);
  	iree_allocator_t host_allocator =
		iree_runtime_session_host_allocator(session);
  	status = iree_ok_status();




	IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
		iree_hal_device_allocator(local_sync_device), IREE_ARRAYSIZE(shape), shape,
		IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
		(iree_hal_buffer_params_t){
			.type = IREE_HAL_MEMORY_TYPE_HOST_LOCAL,
			.usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
		},
		/* initial data */ iree_make_const_byte_span(4, 1024), &input_hal_buffer_view));

	IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
		iree_hal_device_allocator(local_sync_device), IREE_ARRAYSIZE(shape), shape,
		IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
		(iree_hal_buffer_params_t){
			.type = IREE_HAL_MEMORY_TYPE_HOST_LOCAL,
			.usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
		},
		/* initial data */ iree_make_const_byte_span(NULL, 1024), &output_hal_buffer_view));

	
	// delete dynamically allocated lists in call then statically alocate them
	/*
	iree_vm_list_release(module_call.inputs);
	iree_vm_list_release(module_call.outputs);

	iree_vm_type_def_t buffer_view_type_def =
      iree_vm_type_def_make_ref_type(iree_hal_buffer_type_id());

  	iree_byte_span_t input_list_storage = iree_make_byte_span(
      	NULL, iree_vm_list_storage_size(&buffer_view_type_def, 1));
  	IREE_RETURN_IF_ERROR(
      	iree_vm_list_initialize(input_list_storage, &buffer_view_type_def,
                              	1, &module_call.inputs));*/
								
/*
	// adding allocated buffer to call inputs
	// iree_vm_list_t* inputs = iree_runtime_call_inputs(&module_call);
  	iree_vm_ref_t in_ref = {0};
	IREE_RETURN_IF_ERROR(iree_vm_ref_wrap_assign(
      	input_hal_buffer_view, iree_hal_buffer_view_type_id(), &in_ref));
 	iree_vm_list_push_ref_retain(module_call.inputs, &in_ref);
	
	iree_vm_ref_t out_ref = {0};
	IREE_RETURN_IF_ERROR(iree_vm_ref_wrap_assign(
      	output_hal_buffer_view, iree_hal_buffer_view_type_id(), &out_ref));
 	iree_vm_list_push_ref_retain(module_call.outputs, &out_ref);
*/


	
	//iree_runtime_session_retain(session);
    fprintf(stdout, "\n * \n");

	return true;
}



void iree_runtime_render(BelaContext* context, void* userData){
	//iree_vm_list_retain(module_call.inputs);
	// iree_vm_list_t* inputs = iree_runtime_call_inputs(&module_call);
	iree_runtime_call_inputs_push_back_buffer_view(
	&module_call, input_hal_buffer_view);

	// list and hal buffer view are NULL?
	//

	iree_hal_buffer_view_t* input_buffer_view = (iree_hal_buffer_view_t*)iree_vm_list_get_ref_deref(module_call.inputs, 0, iree_hal_buffer_view_get_descriptor());
	//iree_hal_buffer_map_write(
	//	iree_hal_buffer_view_buffer(input_buffer_view),
	//	0, context->audioIn, context->audioFrames*4);
	// iree_vm_list_release(module_call.inputs);

}

void iree_runtime_cleanup(BelaContext *context, void *userData){
	iree_runtime_instance_release(instance);
	// release runtime instance
}

