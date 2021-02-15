# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( EventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES EventInfo/EventInfo.h EventInfo/EventStreamInfo.h
         EventInfo/MergedEventInfo.h EventInfo/PileUpEventInfo.h
   LINK_LIBRARIES AthenaPoolUtilities AthenaKernel AthenaPoolCnvSvcLib EventInfo
                  EventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND AND NOT GENERATIONBASE )
  set( EVENTATHENAPOOL_REFERENCE_TAG
       EventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( EventTPCnv_15.0.0   AOD-15.0.0-full
                  REFERENCE_TAG ${EVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( EventTPCnv_17.5.0   AOD-17.5.0-full
                  REFERENCE_TAG ${EVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( EventTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${EVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( EventTPCnv_20.1.7.2 ESD-20.1.7.2
                  REFERENCE_TAG ${EVENTATHENAPOOL_REFERENCE_TAG} )
elseif( NOT ATHENAPOOLUTILITIESTEST_FOUND )
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
