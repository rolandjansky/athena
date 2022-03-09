# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MMRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO )

# Component(s) in the package:
atlas_add_component( MMRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel MuonAnalysisInterfacesLib MuonCalibIdentifier MuonDQAUtilsLib MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonReadoutGeometry StoreGateLib xAODMuon xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
