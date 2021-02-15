# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( eflowAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( eflowAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES eflowEvent/eflowObjectContainer.h
   LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP
                  eflowEvent eflowEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( EFLOWATHENAPOOL_REFERENCE_TAG
       eflowAthenaPoolReference-01-00-00 )
  run_tpcnv_test( eflowEventTPCnv_17.5.0   AOD-17.5.0-full
                  REFERENCE_TAG ${EFLOWATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( eflowEventTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${EFLOWATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
