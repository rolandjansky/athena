# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTLAMonitoring )

# Component(s) in the package:
atlas_add_component( TrigTLAMonitoring
                     src/TrigTLAMonitorAlgorithm.cxx src/components/TrigTLAMonitoring_entries.cxx
                     LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib xAODJet xAODEgamma xAODMuon )

# Install files from the package:
atlas_install_python_modules( python/TrigTLAMonitorAlgorithm.py python/decodeBS_TLA_AOD.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
