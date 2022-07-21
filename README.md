# Bela IREE runtime

## Prerequisites

- Have IREE Bela development environment setup, see bela-iree-container
- Have models imported into MLIR - TODO: Link to docs

## Design of actual runtime

- Link to sample runtime: https://github.com/iree-org/iree/tree/main/runtime/src/iree/runtime/demo 

- Choice of backend: cpu,gpu,vmvx?
- Choice of implementation: simple_embedding, ...?
- Features: dynamic shapes? statefulness? considerations for sensor/actuator I/O?
- Microkernels? How to 1: identify slow microkernels 2: implement new ones(How much work is this?)

## Perf analysis

- Simple interface for iree-benchmark-modules and iree-tracy-capture
