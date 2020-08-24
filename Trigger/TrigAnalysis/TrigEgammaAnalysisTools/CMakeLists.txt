# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaAnalysisTools )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist Tree )

# Component(s) in the package:
atlas_add_library( TrigEgammaAnalysisToolsLib
   TrigEgammaAnalysisTools/*.h Root/*.cxx
   PUBLIC_HEADERS TrigEgammaAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EgammaAnalysisInterfacesLib GaudiKernel LumiBlockCompsLib LumiBlockData PATCoreLib StoreGateLib TrigConfHLTData TrigDecisionToolLib TrigEgammaEmulationToolLib TrigEgammaMatchingToolLib TrigHLTMonitoringLib TrigNavigationLib xAODCaloEvent xAODCaloRings xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger xAODTruth
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthenaMonitoringLib TrigConfxAODLib TrigSteeringEvent )

atlas_add_component( TrigEgammaAnalysisTools
   src/*.h
   src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaMonitoringLib TrigEgammaAnalysisToolsLib )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_generic( share/trigEgammaDQ.py share/get_trigEgammaDQ.sh 
   DESTINATION share
   EXECUTABLE )
