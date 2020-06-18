# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaEmulationTool )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigEgammaEmulationToolLib
   TrigEgammaEmulationTool/*.h Root/*.cxx
   PUBLIC_HEADERS TrigEgammaEmulationTool 
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools AthContainers EgammaAnalysisInterfacesLib InDetTrackSelectionToolLib LumiBlockCompsLib PATCoreLib RecoToolInterfaces TrigConfHLTData TrigDecisionToolLib TrigEgammaMatchingToolLib TrigMultiVarHypoLib xAODBase xAODCaloEvent xAODCaloRings xAODEgamma xAODPrimitives xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger
   PRIVATE_LINK_LIBRARIES StoreGateLib TrigSteeringEvent )

atlas_add_component( TrigEgammaEmulationTool
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigEgammaEmulationToolLib )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/test*.py )
