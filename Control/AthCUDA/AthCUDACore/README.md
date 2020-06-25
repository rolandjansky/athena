# Core CUDA Code

This package collects all core "helper" CUDA code. Code that is only ever
used on the host to interact with (a) device (memory).

It is separated from `AthCUDAInterfaces` mainly to be able to build it as a
shared library.
