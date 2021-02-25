# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigmuComb )

# Component(s) in the package:
atlas_add_component( TrigmuComb
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel MagFieldConditions StoreGateLib TrigInDetEvent TrigMuonEvent TrkExInterfaces xAODTracking xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
