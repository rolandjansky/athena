The ATLAS Derivation Software Project
=====================================

This is the configuration for building the derivation software of ATLAS,
meant for producing DAOD files from the AOD files reconstructed by the
full Athena project.

It shares much the same code with the Athena project, it just doesn't
build a lot of things that are not needed for the derivation jobs.

Build Instructions
------------------

To build the externals necessary for building this project itself, use the

    build_externals.sh

script. It will build all the externals necessary for this project into a
subdirectory of the directory holding this repository, called `build`.

The sources of the externals will be checked out under `build/src`, the
build of the projects will commence under `build/build`, and the results of
the build will be installed under `build/install`.

RPMs created from the externals are copied under `build/` by the script.

Once the externals have finished building, you can initiate the full build
of the project against these newly built externals by executing the

    build.sh

script. It uses the same directory layout inside the `build` directory as
was used for the externals.

Custom Builds
-------------

Of course it is perfectly allowed to set up a build by hand, not using the
`build.sh` script as well. In that case you have to make sure to have a
functional version of AthenaExternals set up in your environment, and point
the `GAUDI_ROOT` environment variable against the Gaudi version that you
want to use for the build.

To perform a build against locally built externals, the simplest is to just
source the `build_env.sh` script from the environment that you built the
externals from. Or, if you want to build against externals installed with
a release/nightly, just use `asetup` to set up that release/nightly.
