# NOTE: not using bazel-to-cmake here because of the runtime unified rule.
# We should figure out how to make bazel/cmake consistent with that.
cmake_minimum_required(VERSION 3.23)
#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(BELA_IREE_RUNTIME)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Finding dependencies
include(FetchContent)
FetchContent_Declare(
	iree
	BINARY_DIR /opt/iree-device-build/
	SOURCE_DIR /home/iree/
	)

    list(APPEND CMAKE_MODULE_PATH "/home/iree/third_party/llvm-project/mlir/cmake/modules/")

FetchContent_MakeAvailable(iree)
FetchContent_GetProperties(iree SOURCE_DIR IREE_SOURCE_DIR)



find_library(COBALT_LIB REQUIRED
    NAMES cobalt libcobalt
    HINTS "/sysroot/usr/xenomai/lib"
)

find_library(NEON_LIB REQUIRED
    NAMES NE10 libNE10
    HINTS "/sysroot/usr/lib"
)

find_library(MATHNEON_LIB REQUIRED
    NAMES mathneon libmathneon
    HINTS "/sysroot/usr/lib"
)





# Setting default IREE options
set(IREE_BUILD_COMPILER OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
# set(CMAKE_TOOLCHAIN_FILE "/home/cmake/Toolchain.cmake")
set(IREE_HOST_BINARY_ROOT /opt/iree-host-build)
set(IREE_DEVICE_SIZE "uint32_t")
set(IREE_TARGET_BACKEND_DEFAULTS OFF)
set(IREE_TARGET_BACKEND_DYLIB_LLVM_AOT ON)
set(IREE_HAL_DRIVER_DEFAULTS OFF)
set(IREE_HAL_DRIVER_LOCAL_SYNC ON)
set(IREE_HAL_EXECUTABLE_LOADER_EMBEDDED_ELF ON)


set(_NAME "bela-iree-runtime")
add_executable(${_NAME} "")
target_sources(${_NAME}
  PRIVATE
    "main.cpp"
    "render.c"
)

add_compile_definitions(DXENOMAI_SKIN_posix)

set_target_properties(${_NAME} PROPERTIES OUTPUT_NAME "bela-iree-runtime")
target_include_directories(${_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_include_directories(${_NAME} PRIVATE /sysroot/root/Bela /sysroot/root/Bela/include)
target_include_directories(${_NAME} PUBLIC /home/iree/)

target_link_libraries(${_NAME}
  iree_base_base
  iree_base_tracing
  iree_hal_hal
  iree_runtime_runtime
  /sysroot/root/Bela/lib/libbelafull.a
  prussdrv
  asound
  atomic
  sndfile
  pthread
  ${COBALT_LIB}
  ${NEON_LIB}
  ${MATH_NEON_LIB}
)


