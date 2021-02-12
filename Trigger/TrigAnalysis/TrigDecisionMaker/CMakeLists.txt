# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionMaker )

# External dependencies:
find_package( Boost )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigDecisionMakerLib
                   TrigDecisionMaker/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigDecisionMaker
                   LINK_LIBRARIES GaudiKernel TrigT1Result )

atlas_add_component( TrigDecisionMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps EventInfo TrigConfData TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigDecisionEvent TrigDecisionMakerLib TrigOutputHandlingLib TrigSteeringEvent TrigT1Interfaces TrigT1Result xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
