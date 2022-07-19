# Bela IREE runtime

## Prerequisites

- IREE binaries installed
- IREE compiler setup w/ built .vmfb files
- v0.5a Bela image (Debian 11?)
- Have compiled the simple embedding samples already
- Tracy profiler


## Design of actual runtime

- Links to vmvx design docs: 

- Choice of backend: cpu,gpu,vmvx?
- Choice of implementation: simple_embedding, ...?
- Features: dynamic shapes? statefulness?
- Microkernels? How to 1: identify slow microkernels 2: implement new ones(How much work is this?)

## Perf analysis

- Simple interface for iree-benchmark-modules and iree-tracy-capture
