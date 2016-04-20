# $Id: CMakeLists.txt 741506 2016-04-20 07:47:54Z krasznaa $
#
# Package building Geant4 with all its ATLAS specific patches, for the offline
# builds.
#

# The name of the package:
atlas_subdir( Geant4 )

# The dependencies of the package:
atlas_depends_on_subdirs( PUBLIC
   External/AtlasCLHEP )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# The externals needed for the build. Note that we could actually
# skip finding the packages here. We just look for them already here
# to ease debugging in case they can't be found later on by Geant4 itself.
find_package( XercesC )
find_package( EXPAT )

# Source for Geant4:
set( _source
   http://geant4.web.cern.ch/geant4/support/source/geant4.10.01.p01.tar.gz )
# The patch for Geant4:
set( _patchDir
   /afs/cern.ch/atlas/offline/external/geant4/g4.10.1/geant4.10.1.patch01.atlas04 )
# Version string to replace:
set( _g4vers_orig "geant4-10-01-patch-01" )
# Version string to use:
set( _g4vers_patch "geant4.10.1.patch01.atlas04" )

# Set some environment variables for the build:
if( APPLE )
   set( ENV{G4SYSTEM} "Darwin-g++" )
else()
   set( ENV{G4SYSTEM} "Linux-g++" )
endif()
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
   set( ENV{G4DEBUG} "1" )
endif()
set( ENV{G4LIB_BUILD_GDML} "1" )
set( ENV{G4LIB_BUILD_SHARED} "1" )
set( ENV{G4VIS_BUILD_DAWN_DRIVER} "1" )
set( ENV{G4VIS_BUILD_DAWNFILE_DRIVER} "1" )
set( ENV{G4VIS_BUILD_VRML_DRIVER} "1" )
set( ENV{G4VIS_BUILD_VRMLFILE_DRIVER} "1" )
set( ENV{G4INSTALL} "${CMAKE_CURRENT_BINARY_DIR}/build" )

# Extra options for the configuration:
set( _extraOptions )
if( "${CMAKE_CXX_STANDARD}" EQUAL 11 )
   list( APPEND _extraOptions -DGEANT4_BUILD_CXXSTD=c++11 )
elseif( "${CMAKE_CXX_STANDARD}" EQUAL 14 )
   list( APPEND _extraOptions -DGEANT4_BUILD_CXXSTD=c++1y )
endif()
if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
   list( APPEND _extraOptions -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )
endif()

# Get the number of cores in the build machine. Building G4 using all the cores
# overcommits the build machine a bit, but that should be okay. This is by far
# the largest external built in the offline software, so everything else should
# build by the time that this is done, even if they can only use the "spare"
# CPU cycles.
atlas_cpu_cores( nCPUs )

# Build Geant4 for the build area:
ExternalProject_Add( Geant4
   PREFIX ${CMAKE_BINARY_DIR}
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   URL ${_source}
   PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${_patchDir}/ <SOURCE_DIR>
   COMMAND sed "s/${_g4vers_orig}/${_g4vers_patch}/g"
   <SOURCE_DIR>/include/G4Version.hh > <SOURCE_DIR>/include/G4Version_new.hh
   COMMAND ${CMAKE_COMMAND} -E rename <SOURCE_DIR>/include/G4Version_new.hh
   <SOURCE_DIR>/include/G4Version.hh
   COMMAND sed "s/${_g4vers_orig}/${_g4vers_patch}/g"
   <SOURCE_DIR>/source/global/management/include/G4Version.hh >
   <SOURCE_DIR>/source/global/management/include/G4Version_new.hh
   COMMAND ${CMAKE_COMMAND} -E rename
   <SOURCE_DIR>/source/global/management/include/G4Version_new.hh
   <SOURCE_DIR>/source/global/management/include/G4Version.hh
   CMAKE_CACHE_ARGS
   -DGEANT4_USE_GDML:BOOL=ON
   -DGEANT4_USE_SYSTEM_ZLIB:BOOL=OFF
   -DCLHEP_ROOT_DIR:PATH=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   -DXERCESC_ROOT_DIR:PATH=${XERCESC_ROOT}
   -DGEANT4_USE_SYSTEM_CLHEP:BOOL=ON
   -DGEANT4_USE_SYSTEM_CLHEP_GRANULAR:BOOL=ON
   -DGEANT4_BUILD_MULTITHREADED:BOOL=ON
   -DGEANT4_BUILD_TLS_MODEL:STRING=global-dynamic
   -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/build
   -DCMAKE_INSTALL_LIBDIR:STRING=lib
   -DCMAKE_PREFIX_PATH:STRING=${EXPAT_ROOT}
   -DGEANT4_INSTALL_DATA:BOOL=OFF
   CMAKE_ARGS
   ${_extraOptions}
   LOG_CONFIGURE 1
   BUILD_COMMAND make -j${nCPUs} )
ExternalProject_Add_Step( Geant4 buildinstall
   COMMAND ${CMAKE_COMMAND} -E remove_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/lib/Geant4-10.1.1
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   COMMENT "Installing Geant4 into the build area"
   DEPENDEES install )
add_dependencies( Package_Geant4 Geant4 )
add_dependencies( Geant4 CLHEP )

# Install Geant4:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
