# The ATLAS DetCommon Mini-Project

This is the configuration for building the DetCommon project that contains
code to read the trigger configuration. It only has `tdaq-common` and `LCG`
dependencies. The project is e.g. used at P1 to configure the L1 hardware (CTP, L1Calo).

## Use with tdaq

If the project is used in conjuntion with the `tdaq` project a typical 
CMake configuration could look like this:
```cmake
tdaq_project( MyProject 1.0.0 USES tdaq 9.2.1 DetCommon 22.0.30 )
```

## Build Instructions

The build procedure for this project is the same as for all the other ones.
You first execute the

    build_externals.sh

script with its "usual arguments", followed by the

    build.sh

script. As in the other cases the scripts will create a `build` directory
with the same layout as the scripts in the Athena project do.

The main difference is that DetCommon, unlike any of the other projects
in the repository, is built as a single project. There is no
`DetCommonExternals` project for it. So the `build_externals.sh` script
actually just downloads the correct version of
[atlas/atlasexternals](https://gitlab.cern.ch/atlas/atlasexternals), as it is
still needed for the build, but doesn't build a project out of it. That
repository is instead used directly while building this project using
`build.sh`.

### Build on ARM
Building on ARM currently requires a few extra steps. First, make sure that you have a suitable version of `gcc` and `cmake`
available on your build machine. If you have `cvmfs` access, you can also use the centrally provided ones:
```
asetup none,gcc8,cmakesetup
export PATH=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/aarch64-Linux/Cmake/current/Linux-aarch64/bin:$PATH
```
Then proceed with the build:
```
./athena/Projects/DetCommon/build_externals.sh
export TDAQ_RELEASE_BASE=/cvmfs/atlas-online-nightlies.cern.ch/tdaq/arm-native
./athena/Projects/DetCommon/build.sh -x -DLCG_VERSION_POSTFIX=arm
```

## Project Versioning Scheme

When in production, the project is usually built at the same time as the
Athena project and carries the same (3-digit) version number. If a patch
is needed (without building Athena) a 4-digit version number should be used.

