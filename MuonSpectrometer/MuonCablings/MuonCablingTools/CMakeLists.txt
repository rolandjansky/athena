# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCablingTools )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( MuonCablingTools
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingTools
                   LINK_LIBRARIES GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaKernel )

# Tests:
atlas_add_test( dbline_test
                SOURCES test/dbline_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} MuonCablingTools )
