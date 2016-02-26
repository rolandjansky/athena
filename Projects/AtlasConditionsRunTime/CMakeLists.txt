# $Id: CMakeLists.txt 726448 2016-02-26 15:14:42Z krasznaa $
#
# This package is meant to pull in runtime dependencies that are not needed
# to build the AtlasConditions project using CMake, but are necessary to
# run jobs using the project.
#

# Declare a name for the package:
atlas_subdir( AtlasConditionsRunTime )

# Externals that no package needs for its build:
find_package( Frontier_Client )
