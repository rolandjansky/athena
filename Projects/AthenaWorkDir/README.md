AthenaWorkDir Project
=====================

This project can be used to build just a few packages from the repository
against an installed Athena release/nightly. It can be used in two ways.

Using a fully checked out repository
------------------------------------

If you have the entire `athena` repository checked out, you need to tell
the CMake configuration which packages you actually want to build. Otherwise
it will go ahead and attempt to build everything. Even packages not part of
the full Athena release itself.

You should make a copy of the `package_filters_example.txt` file found in
this directory, and modify it to select the packages you're interested in,
for compilation. Then, configure the build of the project like:

    asetup Athena,...
    mkdir build
    cd build/
    cmake -DATLAS_PACKAGE_FILTER_FILE=/your/specific/package_filters.txt \
       ../athena/Projects/AthenaWorkDir/

Using a partially checked out repository
----------------------------------------

If you only checked out the packages that you want to compile, along with
this `Projects/AthenaWorkDir` directory, then you don't need to worry about
setting up a package filtering file. You can simply just do:

    asetup Athena,...
    mkdir build
    cd build/
    cmake ../athena/Projects/AthenaWorkDir/

This will set up the build of all checked out packages.

Using the local build
---------------------

After being done with the build, you have to remember to source the
`setup.sh` file of the build area to pick up your modifications! So, you
would do something like this to set up your runtime environment from scratch:

    asetup Athena,...
    source build/x86_64-slc6-gcc49-opt/setup.sh

Current nightly to use
----------------------

This subsection will be out of date very quickly, at which point it should
just be removed. But for now you can set up the Athena nightly from CVMFS
in the following way:

    setupATLAS
    lsetup -a latest asetup
    asetup Athena,22.0.X-GIT,r15
