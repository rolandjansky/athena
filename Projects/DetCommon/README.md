The ATLAS DetCommon Mini-Project
===================================

This is the configuration for building the DetCommon project that is
used at P1 to configure the L1 hardware (CTP, L1Calo).

Build Instructions
------------------

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

Project Versioning Scheme
-------------------------

When in production, the version number of this project goes hand in hand with
the version number of the Athena project.
