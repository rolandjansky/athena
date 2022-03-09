#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTauRec )

# Component(s) in the package:
atlas_add_component( TrigTauRec
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthAnalysisBaseCompsLib AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel StoreGateLib TrigSteeringEvent tauRecToolsLib xAODJet xAODTau xAODTracking BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

atlas_add_test( TrigTauFlagsTest
   SCRIPT python -m TrigTauRec.TrigTauConfigFlags
   POST_EXEC_SCRIPT nopost.sh )