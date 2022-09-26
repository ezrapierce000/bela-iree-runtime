# Bela IREE runtime

This repository is an example IREE runtime for Bela. See https://github.com/iree-org/iree & https://github.com/iree-org/iree/tree/main/docs/developers/design_docs for more IREE runtime information. This runtime is not yet safe to use.

At present, the runtime has two options. Either loading a vmfb file on setup or compiling in a `module.c` file ahead of time (currently on the emit-c branch). The emit-c branch also has the option to print out Xenomai diagnostics

## Prerequisites

- v0.5a Bela image
- Bela IREE cross compilation toolchain, see [bela-iree-container](https://github.com/ezrapierce000/bela-iree-container)
- A compiled VMFB file, with matching file and function names to those in `render.c` as well as matching input/output shapes (these can all be edited as you wish).

## Future work

- Implement downsampling mechanism to allow for models to run at arbitrary sample rates in the background without modifying the codec's sample rate.
- Split off model-specific information into a header file for easier configuration.
- Add more profiling options on top of `cobalt_threadstat()`, possibly Tracy instrumentation from IREE.


