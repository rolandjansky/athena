#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMETMonitoring )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigMETMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel MissingETEvent StoreGateLib TrigConfL1Data TrigDecisionInterface TrigHLTMonitoringLib TrigMissingEtEvent xAODEgamma xAODEventInfo xAODMissingET xAODMuon xAODTrigMissingET xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

