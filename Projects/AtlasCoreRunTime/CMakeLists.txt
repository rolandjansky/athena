# $Id: CMakeLists.txt 726577 2016-02-27 09:55:50Z krasznaa $
#
# Package listing the runtime dependencies (not set up by regular packages)
# of the AtlasCore project.
#

# The name of the package:
atlas_subdir( AtlasCoreRunTime )

# Find the necessary packages, which sets them up for the runtime
# environment:
find_package( pytools )
find_package( pyanalysis )
find_package( pygraphics )
