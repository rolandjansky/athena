# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTriggerAlgs )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( AnalysisTriggerAlgs
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib
   EventInfo xAODTrigL1Calo xAODTrigger GaudiKernel AnalysisTriggerEvent
   TrigConfData TrigConfL1Data TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Interfaces
   TrigT1Result TrigConfInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
