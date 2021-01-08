# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerD3PDMaker )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TriggerD3PDMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS TriggerD3PDMaker
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils D3PDMakerInterfaces D3PDMakerUtils EventKernel FourMomUtils GaudiKernel TrigAnalysisInterfaces TrigDecisionToolLib TrigObjectMatchingLib TrigT1Result
                   PRIVATE_LINK_LIBRARIES AnalysisTriggerEvent StoreGateLib TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigMonitoringEvent TrigSteeringEvent TrigT1Interfaces xAODTrigger )

atlas_add_component( TriggerD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES TriggerD3PDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

