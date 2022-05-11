# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMinBiasMonitoring )

# Component(s) in the package:
atlas_add_component( TrigMinBiasMonitoring
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel InDetTrackSelectionToolLib StoreGateLib TrigDecisionToolLib xAODEventInfo xAODForward xAODTracking xAODTrigMinBias xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# TODO restore once the new AOD wiht new metadata is available in test files
# atlas_add_test( ConfigTest
#             SCRIPT  python -m  TrigMinBiasMonitoring.TrigMinBiasMonitoringMT 
#             POST_EXEC_SCRIPT nopost.sh)
