# $Id: CMakeLists.txt 742632 2016-04-25 13:17:33Z krasznaa $
#
# Package building GDB as part of the offline software build.
#

# The name of the package:
atlas_subdir( AtlasGdb )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Contents of the configuration script. Note that it's extremely difficult
# to use quotes of any kind inside of ExternalProject_Add, in conjunction
# with the way we log the build messages in the nightly. So just constructing
# a script that executes the configuration for us, is the most robust thing
# to do.
set( _cmd "sh -c \"./configure" )
set( _cmd "${_cmd} --prefix=${CMAKE_CURRENT_BINARY_DIR}/build" )
set( _cmd
   "${_cmd} --with-system-gdbinit=${CMAKE_CURRENT_BINARY_DIR}/build/etc/gdbinit" )
set( _cmd "${_cmd} --with-python=${PYTHON_ROOT} --enable-install-libiberty" )
set( _cmd "${_cmd} LDFLAGS=-L${PYTHON_ROOT}/lib" )
set( _cmd "${_cmd} CFLAGS=\\\"-fPIC -Wno-implicit-function-declaration\\\"" )
set( _cmd "${_cmd} CXXFLAGS=-fPIC\"" )
file( GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.sh
   CONTENT ${_cmd} )

# Set up the build of GDB for the build area:
ExternalProject_Add( gdb
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/gdb-7.11.tar.xz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   BUILD_IN_SOURCE 1
   CONFIGURE_COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/configure.sh
   BUILD_COMMAND make
   COMMAND make install
   INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   )
add_dependencies( Package_AtlasGdb gdb )

# Set up its installation:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )

# Libraries to link against:
set( _libraries
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}bfd${CMAKE_STATIC_LIBRARY_SUFFIX}
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib64/${CMAKE_STATIC_LIBRARY_PREFIX}iberty${CMAKE_STATIC_LIBRARY_SUFFIX} )

# Build the package's application:
atlas_add_executable( resolveAtlasAddr2Line app/resolveAtlasAddr2Line.cpp
   INCLUDE_DIRS ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
   LINK_LIBRARIES ${_libraries} z )
add_dependencies( resolveAtlasAddr2Line gdb )

# Install its other resources:
atlas_install_python_modules( python/*.py python/gdbhacks )
atlas_install_scripts( share/atlasAddress2Line )
