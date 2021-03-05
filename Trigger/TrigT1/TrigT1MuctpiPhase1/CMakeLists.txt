# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1MuctpiPhase1 )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigT1MuctpiPhase1Lib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1MuctpiPhase1
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES AnalysisTriggerEvent AthenaBaseComps CxxUtils GaudiKernel PathResolver StoreGateLib TrigConfInterfaces TrigConfData TrigConfL1Data TrigConfMuctpi TrigT1Interfaces TrigT1Result xAODTrigger
                   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} )

atlas_add_component( TrigT1MuctpiPhase1
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1MuctpiPhase1Lib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
