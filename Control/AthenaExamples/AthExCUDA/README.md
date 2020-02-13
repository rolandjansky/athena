# CUDA Example Package

This package is meant to hold code demonstrating how to use CUDA directly
from Athena.

For the package to "do anything", CUDA needs to be available in the build
environment. Note that it does not necessarily need to be available in
the runtime environment, as by default CMake links the package's library
against the CUDA runtime library statically.

CUDA may be available on lxplus in other locations as well, but one place
that should be available for now is:

```
/afs/cern.ch/work/k/krasznaa/public/cuda
```
