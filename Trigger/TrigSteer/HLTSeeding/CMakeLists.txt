# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HLTSeeding )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( HLTSeedingLib
                   PUBLIC_HEADERS HLTSeeding
                   INTERFACE 
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigConfData )
                   
atlas_add_component( HLTSeeding
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthenaMonitoringKernelLib
                     GaudiKernel HLTSeedingLib StoreGateLib TrigCompositeUtilsLib TrigConfxAODLib TrigCostMonitorLib
                     TrigSteeringEvent TrigT1Interfaces TrigT1Result TrigTimeAlgsLib xAODEventInfo xAODTrigger xAODCore )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( HLTSeedingNJOTest
                SCRIPT python -m HLTSeeding.HLTSeedingConfig
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 300 )
