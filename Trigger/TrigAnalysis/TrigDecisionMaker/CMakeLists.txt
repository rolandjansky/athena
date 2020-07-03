# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionMaker )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrigDecisionMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaBaseComps EventInfo GaudiKernel StoreGateLib TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigDecisionEvent TrigOutputHandlingLib TrigSteeringEvent TrigSteeringLib TrigT1Result xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
