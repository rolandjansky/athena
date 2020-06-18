# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaMatchingTool )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigEgammaMatchingToolLib
                   Root/*.cxx
                   PUBLIC_HEADERS TrigEgammaMatchingTool
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigSteeringEvent xAODRootAccess
                   LINK_LIBRARIES AsgTools DecisionHandlingLib GaudiKernel TrigConfHLTData TrigDecisionToolLib xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigger )

atlas_add_component( TrigEgammaMatchingTool
                     src/*.h src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringLib GaudiKernel StoreGateLib TrigEgammaMatchingToolLib TriggerMatchingToolLib xAODMuon xAODTau )
