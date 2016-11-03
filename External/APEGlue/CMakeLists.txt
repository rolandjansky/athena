# $Id: CMakeLists.txt 782163 2016-11-03 15:44:29Z kama $
#
# CMake configuration building APE as part of the release build.
# To be kept in sync with the requirements file of the package.
#

# Set the name of the package:
atlas_subdir( APEGlue )

# Set up its package dependencies:
atlas_depends_on_subdirs(
   PRIVATE Externals/yampl )
   
# External dependencies: 
find_package( TBB )   

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Get the offline repository location, if it's set in the environment:
if( ENV{SVNROOT} )
   set( _svnroot $ENV{SVNROOT} )
else()
   set( _svnroot "svn+ssh://svn.cern.ch/reps/atlasoff" )
endif()

message(STATUS "SAMI TBBPAth=${TBB_INSTALL_DIR}")
# Build the package for the build area:
ExternalProject_Add( APE
   PREFIX ${CMAKE_BINARY_DIR}
   SVN_REPOSITORY ${_svnroot}/Offloading/APE/tags/APE-01-02-00
   CMAKE_CACHE_ARGS
   -DTBB_INSTALL_DIR:PATH=${TBB_DIR}
   -DYAMPL_DIR:PATH=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   -DBOOST_ROOT:PATH=${BOOST_ROOT}
   -DBOOST_INCLUDEDIR:PATH=${BOOST_INCLUDEDIR}
   -DCMAKE_PREFIX_PATH:PATH=${YAMLCPP_ROOT}
   -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/build
   -DCMAKE_SYSTEM_IGNORE_PATH:STRING=/usr/include;/usr/lib;/usr/lib32;/usr/lib64
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   LOG_CONFIGURE 1 )
ExternalProject_Add_Step( APE buildinstall
   COMMAND ${CMAKE_COMMAND} -E remove_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/lib/cmake
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   COMMENT "Installing APE into the build area"
   DEPENDEES install )
add_dependencies( APE yampl )
add_dependencies( Package_APEGlue APE )

# Install APE:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
