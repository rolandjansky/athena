# Basic Types and Interfaces for CUDA

This package collects some of the basic types and abstract interfaces used in
the management of [CUDA](https://developer.nvidia.com/cuda-zone) code in
higher level packages.

This code is handled separately, as it doesn't need to rely on CUDA at all.
Plus, even more importantly, the `AthCUDA::StreamHolder` code must be held by
a [CMake object library](https://cmake.org/cmake/help/latest/command/add_library.html#object-libraries).
At least with CUDA <= 10.2. :frowning:
