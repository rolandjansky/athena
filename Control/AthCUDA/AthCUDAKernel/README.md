# Helper Code for Creating CUDA GPU Tasks

This package collects (highly-)templated code for creating objects implementing
the `AthCUDA::IKernelTask` interface, which could be executed through
`AthCUDA::IKernelRunnerSvc` at runtime.

Using this code is not mandatory in order to make use of `AthCUDA::IKernelTask`
and/or `AthCUDA::IKernelRunnerSvc`, but since some of that coding tends to be
non-trivial, it makes sense to implement it centrally in an abstract way as
much as possible.
