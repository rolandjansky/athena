# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RpcRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Component(s) in the package:
atlas_add_component( RpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringKernelLib AthenaMonitoringLib EventInfo FourMomUtils GaudiKernel MuonDQAUtilsLib MuonGeoModelLib MuonIdHelpersLib MuonPrepRawData MuonRDO MuonReadoutGeometry MuonTrigCoinData RPC_CondCablingLib StoreGateLib TrigDecisionToolLib TrkEventPrimitives TrkExInterfaces TrkParameters xAODEventInfo xAODMuon xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( python/*.py )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
