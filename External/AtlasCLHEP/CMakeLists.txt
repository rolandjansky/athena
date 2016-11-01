# $Id: CMakeLists.txt 781619 2016-11-01 16:24:31Z jchapman $
#
# Package building CLHEP as part of the offline build procedure.
#


# The name of the package:
atlas_subdir( AtlasCLHEP )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Git repository for CLHEP:
set( _repository https://gitlab.cern.ch/atlas-sw-git/CLHEP.git )
# Git tag to build:
set( _tag "CLHEP_2_2_0_4_atl01")

## The source to build:
#set( _source
#     http://atlas-pmb.web.cern.ch/atlas-pmb/slc6/clhep-2.2.0.4.tgz  )

# Extra options for the configuration:
set( _extraOptions )
if( "${CMAKE_CXX_STANDARD}" GREATER 11 OR
      "${CMAKE_CXX_STANDARD}" EQUAL 11 )
   list( APPEND _extraOptions -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
      -DCLHEP_BUILD_CXXSTD=ON )
endif()

if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
   list( APPEND _extraOptions -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )
endif()

# Get the number of cores in the build machine. Building CLHEP using all the
# cores overcommits the build machine a bit, but that should be okay.
atlas_cpu_cores( nCPUs )

# Buiild CLHEP in the build area:
ExternalProject_Add( CLHEP
   PREFIX ${CMAKE_BINARY_DIR}
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   GIT_REPOSITORY ${_repository}
   GIT_TAG ${_tag}
   PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory
   <SOURCE_DIR>/CLHEP/ <SOURCE_DIR>
   COMMAND ${CMAKE_COMMAND} -E remove_directory <SOURCE_DIR>/CLHEP
   CMAKE_CACHE_ARGS
   -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/build
   -DCLHEP_BUILD_DOCS:BOOL=OFF
   CMAKE_ARGS
   ${_extraOptions}
   LOG_CONFIGURE 1
   BUILD_COMMAND make -j${nCPUs} )
ExternalProject_Add_Step( CLHEP buildinstall
   COMMAND ${CMAKE_COMMAND} -E remove_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/lib/CLHEP-2.2.0.4
   COMMAND ${CMAKE_COMMAND} -E remove_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/lib/pkgconfig
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   COMMENT "Installing CLHEP into the build area"
   DEPENDEES install )
add_dependencies( Package_AtlasCLHEP CLHEP )

# Install CLHEP:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
