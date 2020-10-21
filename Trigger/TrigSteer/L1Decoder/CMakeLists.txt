# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1Decoder )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( L1DecoderLib
                   L1DecoderRoot/*.cxx
                   PUBLIC_HEADERS L1Decoder
                   INTERFACE 
                   LINK_LIBRARIES TrigConfInterfaces)
                   
atlas_add_component( L1Decoder
                     src/*.cxx src/components/*.cxx L1DecoderRoot/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthViews AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib GaudiKernel L1DecoderLib StoreGateLib TrigCompositeUtilsLib TrigConfData TrigConfHLTData TrigConfL1Data TrigCostMonitorMTLib TrigSteeringEvent TrigT1Interfaces TrigT1Result TrigTimeAlgsLib xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( L1DecoderNJOTest SCRIPT python -m L1Decoder.L1DecoderConfig
                PROPERTIES TIMEOUT 300 )
