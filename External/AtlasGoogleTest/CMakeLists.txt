# $Id: CMakeLists.txt 727050 2016-03-01 14:21:44Z krasznaa $
#
# CMake configuration for building GoogleTest/GoogleMock as part of the
# release build.
#

# The name of the package:
atlas_subdir( AtlasGoogleTest )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# The build needs python:
find_package( PythonInterp )

# Extra option(s) for the configuration:
set( _extraOptions )
if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
   list( APPEND _extraOptions -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )
endif()

# Build the package for the build area:
ExternalProject_Add( AtlasGoogleTest
   PREFIX ${CMAKE_BINARY_DIR}
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG ff07a5d
   CMAKE_CACHE_ARGS
   -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/build
   -DBUILD_GTEST:BOOL=ON
   -DBUILD_GMOCK:BOOL=ON
   -DCMAKE_PREFIX_PATH:STRING=${PYTHON_ROOT}
   CMAKE_ARGS ${_extraOptions}
   LOG_CONFIGURE 1 )
ExternalProject_Add_Step( AtlasGoogleTest buildinstall
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   COMMENT "Installing AtlasGoogleTest into the build area"
   DEPENDEES install )
add_dependencies( Package_AtlasGoogleTest AtlasGoogleTest )

# Install GoogleTest/GoogleMock:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )

# Install the CMake module(s) from the package:
install( FILES cmake/FindGMock.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/modules )
