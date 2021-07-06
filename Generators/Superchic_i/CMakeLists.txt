# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
################################################################################
# Package: Superchic_i
################################################################################

# Declare the package name:
atlas_subdir( Superchic_i )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Apfel )
find_package( Lhapdf )
find_package( Superchic )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Set Superchic specific environment variable(s).
set( SuperchicEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of SuperchicEnvironment.cmake" )
find_package( SuperchicEnvironment )
