################################################################################
# Package: MadGraphControl
################################################################################

# Declare the package name:
atlas_subdir( MadGraphControl )

# External dependencies:
find_package( MadGraph )
find_package( Boost )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/common/*.py )
atlas_install_generic( share/file/*.dat share/fastjet-config
                       DESTINATION share
                       EXECUTABLE )

# Set up and install the lhapdf-config script.
string( REPLACE "${LCG_RELEASE_BASE}" "\${LCG_RELEASE_BASE}"
   Boost_INCLUDE_PATH "${Boost_INCLUDE_DIR}" )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/share/lhapdf-config.in"
   "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lhapdf-config" @ONLY )
atlas_install_generic(
   "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lhapdf-config"
   DESTINATION "share"
   EXECUTABLE )

# Set up the general runtime environment:
set( MadGraphControlEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of MadGraphControlEnvironment.cmake" )
find_package( MadGraphControlEnvironment )


