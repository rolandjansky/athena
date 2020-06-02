################################################################################
# Package: MadGraphControl
################################################################################

# Declare the package name:
atlas_subdir( MadGraphControl )

# External dependencies:
find_package( MadGraph )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/common/*.py )
atlas_install_generic( share/file/*.dat share/lhapdf-config share/fastjet-config
                       DESTINATION share
                       EXECUTABLE )

# Set up the general runtime environment:
set( MadGraphControlEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of MadGraphControlEnvironment.cmake" )
find_package( MadGraphControlEnvironment )
