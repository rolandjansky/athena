# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_G4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( HGTD_G4_SD
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} StoreGateLib
   HGTD_Identifier InDetSimEvent G4AtlasToolsLib MCTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
