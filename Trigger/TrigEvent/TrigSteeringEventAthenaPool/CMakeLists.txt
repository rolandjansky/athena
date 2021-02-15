# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteeringEventAthenaPool )

# Extra headers, when not building AthAnalysisBase:
set( extra_headers )
if( NOT XAOD_ANALYSIS )
   set( extra_headers TrigSteeringEvent/Lvl1Result.h
      TrigSteeringEvent/HLTResult.h )
endif()

# Component(s) in the package:
atlas_add_poolcnv_library( TrigSteeringEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrigSteeringEvent/TrigRoiDescriptor.h
   TrigSteeringEvent/TrigRoiDescriptorCollection.h
   TrigSteeringEvent/TrigPassBits.h TrigSteeringEvent/TrigPassBitsCollection.h
   TrigSteeringEvent/TrigPassFlags.h TrigSteeringEvent/TrigPassFlagsCollection.h
   TrigSteeringEvent/GenericResult.h TrigSteeringEvent/TrigOperationalInfo.h
   TrigSteeringEvent/TrigOperationalInfoCollection.h
   ${extra_headers}
   TYPES_WITH_NAMESPACE HLT::HLTResult LVL1CTP::Lvl1Result
   LINK_LIBRARIES AthenaPoolUtilities GaudiKernel TrigSteeringEvent
   AthenaPoolCnvSvcLib TrigSteeringEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRIGSTEERINGEVENTATHENAPOOL_REFERENCE_TAG
       TrigSteeringEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TrigSteeringEventAthenaPool_15.6.1   ESD-15.6.1
                  REFERENCE_TAG ${TRIGSTEERINGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigSteeringEventAthenaPool_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${TRIGSTEERINGEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigSteeringEventAthenaPool_20.1.7.2 ESD-20.1.7.2
                  REFERENCE_TAG ${TRIGSTEERINGEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
