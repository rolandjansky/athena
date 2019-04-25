The ATLAS Data Quality Mini-Project
===================================

This is the configuration for building the AthDataQuality project that is
used in ATLAS's data quality monitoring.

Build Instructions
------------------

The build procedure for this project is the same as for all the other ones.
You first execute the

    build_externals.sh

script with its "usual arguments", followed by the

    build.sh

script. As in the other cases the scripts will create a `build` directory
with the same layout as the scripts in the Athena project do.

The main difference is that AthDataQuality, unlike any of the other projects
in the repository, is built as a single project. There is no
`AthDataQualityExternals` project for it. So the `build_externals.sh` script
actually just downloads the correct version of
[atlas/atlasexternals](https://gitlab.cern.ch/atlas/atlasexternals), as it is
still needed for the build, but doesn't build a project out of it. That
repository is instead used directly while building this project using
`build.sh`.

Project Versioning Scheme
-------------------------

When in production, the version number of this project goes hand in hand with
the version number of the Athena project. The scheme used in the 21.0 branch
is/was:
  - Whenever PROC builds a new Athena 21.0.X release, AthDataQuality is also
    updated by PROC to 21.0.X.1;
  - Whenever the DQ team builds a new AthDataQuality 21.0.X.Y release, the
    AthDataQuality version is updated to 21.0.X.(Y+1).
