The AthSimulation Project
=======================================

This is the configuration for building the parts of the offline
software necessary to run G4 simulation from the repository.

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
functional version of AthSimulationExternals set up in your environment, and point
the `GAUDI_ROOT` environment variable against the Gaudi version that you
want to use for the build.
