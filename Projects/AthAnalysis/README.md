The ATLAS Analysis Software Project
===================================

This is the configuration for building the analysis software of ATLAS,
meant for performing final-stage analysis on DxAOD

It shares much the same code with the Athena project, it just doesn't
build a lot of things that are not needed for analysis jobs.

Setup Instructions
------------------

You need a recent version of cmake and gcc set up. You can get this from
setupATLAS with, for instance:

    asetup none,gcc8,cmakesetup --cmakeversion=3.18.3

Build Instructions
------------------

To build the externals necessary for building this project itself, use the

    ./build_externals.sh -fc

script. It will build all the externals necessary for this project into a
subdirectory of the directory holding this repository, called `build`.

The sources of the externals will be checked out under `build/src`, the
build of the projects will commence under `build/build`, and the results of
the build will be installed under `build/install`.

RPMs created from the externals are copied under `build/` by the script.

Once the externals have finished building, you can initiate the full build
of the project against these newly built externals by executing the

    ./build.sh -acmi

script. It uses the same directory layout inside the `build` directory as
was used for the externals.

Finally, if you want to then *use* the release, you need to set it up by
sourcing the setup script in the install area:

    source ../../../build/install/AthAnalysis/*/InstallArea/*/setup.sh

If you do this in a new shell you will need to lsetup cmake and gcc again
though!

In fact, instead of sourcing the setup script directly, you are advised to use
the `asetup` command if it is available to you (which will source it on your
behalf, as well as probably do some other things). E.g. you can do:

    asetup AthAnalysis,22.2.4 --releasesarea=../../../build/install

of course substitute in the correct version number and path to the install area.
