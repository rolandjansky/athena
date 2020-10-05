# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1Result )

# External dependencies:
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigT1Result
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1Result
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaKernel L1TopoRDO TrigT1Interfaces
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( TrigT1ResultDict
                      TrigT1Result/TrigT1ResultDict.h
                      TrigT1Result/selection.xml
                      LINK_LIBRARIES TrigT1Result )

atlas_add_test( TrigT1Result_test
                SOURCES test/type_trait_tests.cxx
                LINK_LIBRARIES TrigT1Result )
