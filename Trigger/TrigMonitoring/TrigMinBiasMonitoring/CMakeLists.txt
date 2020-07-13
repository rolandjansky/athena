# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMinBiasMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigMinBiasMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringKernelLib AthenaMonitoringLib CaloEvent CaloGeoHelpers CaloIdentifier GaudiKernel InDetBCM_RawData InDetTrackSelectionToolLib LUCID_RawEvent TileEvent TrigCaloEvent TrigDecisionToolLib TrigHLTMonitoringLib TrigInDetEvent ZdcEvent ZdcIdentifier xAODEventInfo xAODTracking xAODTrigMinBias xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
