# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# CMake configuration file for the ART package.
#
# author :  Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>, Attila Krasznahorkay <attila.krasznahorkay@cern.ch>

# Declare the package name:
atlas_subdir( ART )

# Declare the package's dependencies: 
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/ART/*.py )
atlas_install_scripts( scripts/*.py scripts/*.sh )

# Set up the general runtime environment (by Attila Krasznahorkay <attila.krasznahorkay@cern.ch>):
set( ARTEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of ARTEnvironmentConfig.cmake" )
find_package( ARTEnvironment )
