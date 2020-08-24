# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigJetMonitoring )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigJetMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisTriggerEvent AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel StoreGateLib TrigDecisionToolLib TrigHLTMonitoringLib TrigSteeringEvent xAODEventInfo xAODJet xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
