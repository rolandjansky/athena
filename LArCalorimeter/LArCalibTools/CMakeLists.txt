# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCalibTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Component(s) in the package:
atlas_add_component( LArCalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloCondBlobObjs CaloDetDescrLib CaloIdentifier EventInfo GaudiKernel LArCOOLConditions LArCablingLib LArElecCalib LArIdentifier LArRawConditions LArRawEvent LArRecConditions LArToolsLib LumiBlockData StoreGateLib TrigDecisionToolLib xAODEventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
