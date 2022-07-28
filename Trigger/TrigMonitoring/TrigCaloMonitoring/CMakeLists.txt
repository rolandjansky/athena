# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloMonitoring )

# Component(s) in the package:
atlas_add_component( TrigCaloMonitoring
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib CxxUtils LumiBlockData StoreGateLib LArRecEvent xAODCaloEvent xAODTrigCalo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
