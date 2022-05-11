# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataQualityTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DataQualityTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel GeoPrimitives InDetIdentifier InDetPrepRawData InDetRawData IsolationSelectionLib LArRawEvent LArRecEvent LUCID_RawEvent LWHists MagFieldConditions MagFieldElements MuonAnalysisInterfacesLib MuonRDO RecBackgroundEvent StoreGateLib TileEvent TrigDecisionToolLib TrigT1Result TrkExInterfaces TrkParameters TrkSpacePoint ZdcEvent ZdcIdentifier ZdcRecLib xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

