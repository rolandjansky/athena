# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1MuctpiPhase1 )

# External pacakge dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrigT1MuctpiPhase1
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES AnalysisTriggerEvent AthenaBaseComps CxxUtils GaudiKernel PathResolver StoreGateLib TrigConfInterfaces TrigConfL1Data TrigConfMuctpi TrigT1Interfaces TrigT1Result xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
