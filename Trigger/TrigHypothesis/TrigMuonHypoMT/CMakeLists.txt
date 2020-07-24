# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonHypoMT )

# Component(s) in the package:
atlas_add_component( TrigMuonHypoMT
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthViews AthenaBaseComps AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib GaudiKernel MuonAnalysisInterfacesLib StoreGateLib TrigCompositeUtilsLib TrigSteeringEvent TrigT1Interfaces xAODMuon xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigMuonHypoMTConfig
                SCRIPT python -m TrigMuonHypoMT.TrigMuonHypoMTConfig
                POST_EXEC_SCRIPT nopost.sh )
