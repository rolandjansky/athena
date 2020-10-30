# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_LocReco )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist RIO )

# Component(s) in the package:
atlas_add_component( AFP_LocReco
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities EventInfo AFP_DigiEv AFP_Geometry AFP_LocRecoEv GaudiKernel GeneratorObjects xAODForward AthLinks)

# Install files from the package:
atlas_install_joboptions( share/*.py )

