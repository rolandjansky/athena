# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventBookkeeperAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( EventBookkeeperAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES EventBookkeeperMetaData/EventBookkeeperCollection.h
         EventBookkeeperMetaData/EventBookkeeper.h
         EventBookkeeperMetaData/SkimDecisionCollection.h
         EventBookkeeperMetaData/SkimDecision.h
   LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib
                  EventBookkeeperMetaData EventBookkeeperTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND AND NOT GENERATIONBASE )
  set( EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG
       EventBookkeeperAthenaPoolReference-01-00-00 )
  run_tpcnv_test( EventBookkeeperTPCnv_16.0.0   AOD-16.0.0-full
                  REFERENCE_TAG ${EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( EventBookkeeperTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG} )
elseif( NOT ATHENAPOOLUTILITIESTEST_FOUND )
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
