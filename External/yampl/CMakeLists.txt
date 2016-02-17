# $Id: CMakeLists.txt 724488 2016-02-17 10:45:05Z krasznaa $
#
# Package building YAMPL for the offline project.
#

# The package's name:
atlas_subdir( yampl )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Set up the build of YAMPL:
ExternalProject_Add( yampl
   PREFIX ${CMAKE_BINARY_DIR}
   GIT_REPOSITORY https://github.com/vitillo/yampl.git
   GIT_TAG 5f9052a
   BUILD_IN_SOURCE 1
   CONFIGURE_COMMAND ./configure
   --prefix=${CMAKE_CURRENT_BINARY_DIR}/build
   BUILD_COMMAND make
   INSTALL_COMMAND make install
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM} )
add_dependencies( Package_yampl yampl )

# Install YAMPL:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )
