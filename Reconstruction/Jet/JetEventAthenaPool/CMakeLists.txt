# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( JetEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES JetEvent/JetMomentMap.h JetEvent/JetSamplingCollection.h
         JetEvent/JetCollection.h JetEvent/JetKeyDescriptor.h
   LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib JetEvent
                  JetEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( JETEVENTATHENAPOOL_REFERENCE_TAG
       JetEventAthenaPoolReference-01-00-01 )
  run_tpcnv_test( JetTPCnv_15.9.0.5   AOD-15.9.0.5-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( JetTPCnv_17.0.0     AOD-17.0.0-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( JetTPCnv_17.2.8     AOD-17.2.8-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
