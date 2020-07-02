# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTauMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigTauMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisUtilsLib AthenaKernel AthenaMonitoringLib EventInfo GaudiKernel LumiBlockCompsLib LumiBlockData StoreGateLib TrigConfL1Data TrigDecisionInterface TrigDecisionToolLib TrigHLTMonitoringLib TrigSteeringEvent TrigTauEmulationLib VxVertex tauEvent xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTau xAODTracking xAODTrigger xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
