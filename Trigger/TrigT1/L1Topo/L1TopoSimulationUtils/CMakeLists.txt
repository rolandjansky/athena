# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoSimulationUtils )

find_package( Boost )

# Component(s) in the package:
atlas_add_library( L1TopoSimulationUtils
   L1TopoSimulationUtils/*.h Root/*.cxx
   PUBLIC_HEADERS L1TopoSimulationUtils
   LINK_LIBRARIES L1TopoEvent )

atlas_add_test( L1TopoSimulationUtils_test
                SOURCES
                test/L1TopoSimulationUtils_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES L1TopoEvent L1TopoSimulationUtils)
