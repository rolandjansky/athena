# $Id: CMakeLists.txt 726052 2016-02-25 08:22:32Z krasznaa $
#
# CMake configuration for the package. For now it just installs the binaries
# and headers as they are in SVN. Later on it should be modified to rather
# build the package as part of the release build.
#

# The name of the package:
atlas_subdir( AtlasShowerDeconstruction )

# In release rebuild mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Install the files into the build area:
add_custom_command( OUTPUT
   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libDeconstruction.so
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM} )
add_custom_target( ShowerDeconstructionInstall ALL
   SOURCES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libDeconstruction.so )
add_dependencies( Package_AtlasShowerDeconstruction
   ShowerDeconstructionInstall )

# Install them into the install area:
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
   DESTINATION . USE_SOURCE_PERMISSIONS
   PATTERN ".svn" EXCLUDE )
