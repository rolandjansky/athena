# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonMonitoring )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( TrigMuonMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AnalysisTriggerEvent AthenaMonitoringLib EventInfo GaudiKernel StoreGateLib TrigAnalysisInterfaces TrigConfHLTData TrigDecisionToolLib TrigHLTMonitoringLib TrigMuonEvent TrigObjectMatchingLib TrigSteeringEvent VxVertex muonEvent xAODMuon xAODPrimitives xAODTracking xAODTrigMuon xAODTrigger xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )

