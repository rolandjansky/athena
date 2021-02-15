# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TileSimEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TileSimEvent/TileHitVector.h
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel
                  TileSimEvent TileSimEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TILESIMEVENTATHENAPOOL_REFERENCE_TAG
       TileSimEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TileSimEventTPCnv_HITS    HITS.04919495._000416
                  REFERENCE_TAG ${TILESIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
