Directory collecting external package declarations
==================================================

This directory is used to collect simple CMake files that get included by
the CMake configuration to set up the locations of all the externals used
for the offline release build.

Each external should be defined by its own CMake file. The files should
have a name <Bla> corresponding to the Find<Bla> module name used to find
the external in question.

The files should define all the variables expected by the Find<Bla> modules,
which normally boil down to variables:

  - `EXTNAME_LCGROOT`
  - `EXTNAME_LCGVERSION`

But some modules may require other variables. In which case the `_LCGROOT`
variable should still be set, to get a nice printout from the Athena
project during the build about the location of the used external.
