# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTruthEventAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigTruthEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrigInDetTruthEvent/TrigInDetTrackTruthMap.h
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TrigInDetEvent
                  AthenaPoolCnvSvcLib RootConversions TrigInDetTruthEvent
                  TrigTruthEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG
       TrigTruthEventAthenaPool-01-00-00 )
  run_tpcnv_test( TrigTruthEventTPCnv_15.8.0   AOD-15.8.0-full
                  REFERENCE_TAG ${TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigTruthEventTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
