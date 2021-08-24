# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MagFieldUtils )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS RIO MathCore Core Tree Hist pthread )

# Component(s) in the package:
atlas_add_component( MagFieldUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MagFieldConditions MagFieldInterfaces StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
