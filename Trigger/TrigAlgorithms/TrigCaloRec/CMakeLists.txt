# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloRec )

# Component(s) in the package:
atlas_add_library( TrigCaloRecLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCaloRec
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaMonitoringLib CaloEvent CaloInterfaceLib CaloRecLib CxxUtils GaudiKernel IRegionSelector LArCablingLib StoreGateLib TrigCaloEvent TrigInterfacesLib TrigT2CaloCommonLib TrigTimeAlgsLib xAODCaloEvent
                   PRIVATE_LINK_LIBRARIES AthContainers AthenaMonitoringKernelLib CaloDetDescrLib CaloGeoHelpers CaloIdentifier CaloUtilsLib EventKernel Identifier LArIdentifier NavFourMom TileConditionsLib TrigSteeringEvent TrigT1CaloByteStreamLib TrigT1Interfaces xAODHIEvent xAODTrigCalo xAODTrigL1Calo )

atlas_add_component( TrigCaloRec
                     src/components/*.cxx
                     LINK_LIBRARIES TrigCaloRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
