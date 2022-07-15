# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PFODQA )

# Component(s) in the package:
atlas_add_component( PFODQA
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringLib xAODCaloEvent xAODEventInfo xAODPFlow xAODTracking PFOHistUtils StoreGateLib TrkValHistUtils xAODTau xAODEgamma xAODMuon )
atlas_install_python_modules( python/*py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( scripts/*py POST_BUILD_CMD ${ATLAS_FLAKE8} )
