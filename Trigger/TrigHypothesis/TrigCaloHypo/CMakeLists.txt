# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloHypo )

# External dependencies:
find_package( tdaq-common COMPONENTS hltinterface )

# Component(s) in the package:
atlas_add_component( TrigCaloHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloInterfaceLib GaudiKernel LArRecConditions LArRecEvent StoreGateLib TrigCaloEvent TrigInterfacesLib TrigSteeringEvent TrigTimeAlgsLib xAODCaloEvent xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
