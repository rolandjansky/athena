################################################################################
# Package: MadGraphControl
################################################################################

# Declare the package name:
atlas_subdir( MadGraphControl )

# External dependencies:
find_package( MadGraph )

# flake8 setup
set( ATLAS_FLAKE8 "flake8_atlas --select ATL,F,E7,E9,W6 --enable-extension ATL902 --extend-ignore E701,E702,E741"
   CACHE STRING "Default flake8 command" )

set( ATLAS_PYTHON_CHECKER "${ATLAS_FLAKE8} --filterFiles AthenaConfiguration"
   CACHE STRING "Python checker command to run during Python module compilation" )

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
