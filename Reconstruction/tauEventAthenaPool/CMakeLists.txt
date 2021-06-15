# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( tauEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES tauEvent/TauJetContainer.h tauEvent/TauDetailsContainer.h
   TYPES_WITH_NAMESPACE Analysis::TauJetContainer Analysis::TauDetailsContainer
   LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel tauEvent
                  tauEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( TAUEVENTATHENAPOOL_REFERENCE_TAG
        tauEventAthenaPoolReference-01-00-01 )
  run_tpcnv_test( tauEventTPCnv_15.0.0    AOD-15.0.0-full
                   REFERENCE_TAG ${TAUEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( tauEventTPCnv_15.6.9    AOD-15.6.9-full
                   REFERENCE_TAG ${TAUEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( tauEventTPCnv_17.2.9.1  AOD-17.2.9.1-full
                   REFERENCE_TAG ${TAUEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( tauEventTPCnv_18.0.0    AOD-18.0.0-full
                   REFERENCE_TAG ${TAUEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
