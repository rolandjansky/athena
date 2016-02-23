# $Id: CMakeLists.txt 725736 2016-02-23 15:03:37Z krasznaa $
#
# Package building Eigen as part of the offline software build.
#

# The name of the package:
atlas_subdir( AtlasEigen )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Build Eigen for the build area:
ExternalProject_Add( Eigen
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/eigen-3.1.3.tar.gz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   PATCH_COMMAND patch -p0 < ${CMAKE_CURRENT_SOURCE_DIR}/cmt/placement-new.patch
   COMMAND patch -p0 < ${CMAKE_CURRENT_SOURCE_DIR}/cmt/typedef-warnings.patch
   COMMAND patch -p0 < ${CMAKE_CURRENT_SOURCE_DIR}/cmt/register-warnings.patch
   CMAKE_CACHE_ARGS
   -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/build
   LOG_CONFIGURE 1 )
ExternalProject_Add_Step( Eigen buildinstall
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   COMMENT "Installing Eigen into the build area"
   DEPENDEES install )
add_dependencies( Package_AtlasEigen Eigen )

# And now install it:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )
