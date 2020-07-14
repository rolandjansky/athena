# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoRDO )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( L1TopoRDO
                   src/*.cxx
                   PUBLIC_HEADERS L1TopoRDO
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthenaKernel )

atlas_add_test( L1TopoRDO_test
                SOURCES
                test/L1TopoRDO_test.cxx
                LINK_LIBRARIES L1TopoRDO )
