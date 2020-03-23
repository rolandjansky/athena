# SYCL Example Package

This package is meant to hold code demonstrating how to use SYCL directly
from Athena.

Note that building/running SYCL code with/on top of Athena is not super
easy for the time being (5th March 2020). In order to compile/run such code
on top of the `x86_64-centos7-gcc8-opt` nightlies/releases, you have 2
choices.
  - After setting up the `x86_64-centos7-gcc8-opt` nightly/release, set up
    a recent version of Clang compiled privately from the `sycl` branch of
    https://github.com/intel/llvm, and compile this package using
    [Projects/WorkDir](Projects/WorkDir) using that compiler. At the time
    of writing a viable compiler exists on lxplus under:

```
/afs/cern.ch/work/k/krasznaa/public/clang/11.0.0-ef68270/x86_64-centos7-gcc8-opt
```

  - Use a [Docker](https://www.docker.com/) image to build/run the code on
    an OS different from [CentOS 7](http://linux.web.cern.ch/linux/centos7/),
    with a CentOS 7 OS and compiler running inside of the image. At the time
    of writing a viable image exists under:

```
gitlab-registry.cern.ch/akraszna/atlas-gpu-devel-env:cuda-10-1-gcc-8-clang-ef68270
```

For more information on that image, see
[its source repository](https://gitlab.cern.ch/akraszna/atlas-gpu-devel-env).
