# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigL2MissingET )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigL2MissingET
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier CxxUtils GaudiKernel IRegionSelector LArCablingLib LArIdentifier LArRecConditions LArRecEvent StoreGateLib TrigInterfacesLib TrigMissingEtEvent TrigNavigationLib TrigSteeringEvent TrigT1Interfaces TrigT2CaloCommonLib TrigTimeAlgsLib xAODEventInfo xAODTrigMissingET )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
