# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMissingETMuon )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigMissingETMuon
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CxxUtils EventInfo GaudiKernel TrigInterfacesLib TrigMissingEtEvent TrigNavigationLib TrigSteeringEvent TrigT1Interfaces xAODMuon xAODTrigMissingET )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
