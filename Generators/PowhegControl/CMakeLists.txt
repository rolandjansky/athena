################################################################################
# Package: PowhegControl
################################################################################

# Declare the package name:
atlas_subdir( PowhegControl )

# Install files from the package:
atlas_install_python_modules( python/*.py python/algorithms python/decorators python/parameters python/processes python/utility )
atlas_install_joboptions( share/common/*.py
                          share/control/*.py )

set( POWHEG_ROOT "/afs/cern.ch/atlas/offline/external/powhegbox" )
set( POWHEG_VERSION "ATLASOTF-00-04-00" )
# Set up the general runtime environment:
set( PowhegEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of PowhegEnvironment.cmake" )
find_package( PowhegEnvironment )


