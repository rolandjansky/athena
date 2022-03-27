# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonHypo )

# Component(s) in the package:
atlas_add_component( TrigMuonHypo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthViews AthenaBaseComps AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib GaudiKernel MuonAnalysisInterfacesLib StoreGateLib TrigCompositeUtilsLib TrigSteeringEvent xAODMuon xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigMuonHypoConfig
                SCRIPT python -m TrigMuonHypo.TrigMuonHypoConfig
                POST_EXEC_SCRIPT nopost.sh )
