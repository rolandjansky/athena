# CUDA Example Package

This package is meant to hold code demonstrating how to use CUDA directly
from Athena.

For the package to "do anything", CUDA needs to be available in the build
environment. This is not the case by default but can be requested by adding
the `cuda` tag to your asetup command, e.g.:
```
asetup Athena,master,latest,cuda
```
Note that CUDA does not necessarily need to be available in
the runtime environment, as by default CMake links the package's library
against the CUDA runtime library statically. 