# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloRec )

# Component(s) in the package:
atlas_add_component( TrigCaloRec
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps AthenaMonitoringKernelLib CaloEvent CaloInterfaceLib CaloRecLib CaloUtilsLib EventKernel GaudiKernel NavFourMom StoreGateLib TileConditionsLib TrigSteeringEvent TrigT2CaloCommonLib xAODCaloEvent xAODCore xAODTrigCalo xAODHIEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )
