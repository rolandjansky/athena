# $Id: CMakeLists.txt 724505 2016-02-17 11:31:55Z krasznaa $
#
# Package configuration for CMake.
#

# The name of the package:
atlas_subdir( AtlasCompilers )

# In release rebuild mode don't do anything:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Install the package's compilers.json file:
install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/compilers.json
   DESTINATION . OPTIONAL )
