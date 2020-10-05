# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigCaloMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringKernelLib AthenaMonitoringLib CaloEvent CaloGeoHelpers CaloIdentifier CaloInterfaceLib GaudiKernel LArCablingLib LArIdentifier LArRecEvent LumiBlockData StoreGateLib TrigCaloEvent TrigCaloRecLib TrigHLTMonitoringLib TrigSteeringEvent TrigT2CaloCommonLib egammaEvent xAODCaloEvent xAODTrigCalo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
