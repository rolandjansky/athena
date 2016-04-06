# $Id: CMakeLists.txt 734319 2016-04-06 12:48:09Z krasznaa $
#
# Package building the FastJet contrib libraries as part of the offline
# software build.
#

# Set the package name:
atlas_subdir( AtlasFastJetContrib )

# Set the package's dependencies:
atlas_depends_on_subdirs( PUBLIC External/AtlasFastJet )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Set up the build of FastJetContrib for the build area:
ExternalProject_Add( FastJetContrib
   PREFIX ${CMAKE_BINARY_DIR}
   URL http://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.022.tar.gz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   CONFIGURE_COMMAND ./configure
   --fastjet-config=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/bin/fastjet-config
   --prefix=${CMAKE_CURRENT_BINARY_DIR}/build CXXFLAGS=-fPIC
   BUILD_IN_SOURCE 1
   BUILD_COMMAND make
   COMMAND make install
   INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   )
add_dependencies( FastJetContrib FastJet )
add_dependencies( Package_AtlasFastJetContrib FastJetContrib )

# Set up its installation:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )
