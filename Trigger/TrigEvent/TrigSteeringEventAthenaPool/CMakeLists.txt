################################################################################
# Package: TrigSteeringEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrigSteeringEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Trigger/TrigEvent/TrigSteeringEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigSteeringEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigSteeringEvent/TrigRoiDescriptor.h TrigSteeringEvent/TrigRoiDescriptorCollection.h TrigSteeringEvent/TrigPassBits.h TrigSteeringEvent/TrigPassBitsCollection.h TrigSteeringEvent/TrigPassFlags.h TrigSteeringEvent/TrigPassFlagsCollection.h TrigSteeringEvent/GenericResult.h TrigSteeringEvent/Lvl1Result.h TrigSteeringEvent/HLTResult.h TrigSteeringEvent/TrigOperationalInfo.h TrigSteeringEvent/TrigOperationalInfoCollection.h
                           TYPES_WITH_NAMESPACE HLT::HLTResult LVL1CTP::Lvl1Result
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel TrigSteeringEvent AthenaPoolCnvSvcLib TrigSteeringEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
set( _dir
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../Database/AthenaPOOL/AthenaPoolUtilities )
if( IS_DIRECTORY ${_dir} )
   set( AthenaPoolUtilitiesTest_DIR ${_dir}/cmake )
endif()
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
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
