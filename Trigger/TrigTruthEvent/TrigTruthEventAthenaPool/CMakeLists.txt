################################################################################
# Package: TrigTruthEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrigTruthEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/RootConversions
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent
                          Trigger/TrigTruthEvent/TrigTruthEventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigTruthEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigInDetTruthEvent/TrigInDetTrackTruthMap.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TrigInDetEvent AthenaPoolCnvSvcLib RootConversions TrigInDetTruthEvent TrigTruthEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG
       TrigTruthEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TrigTruthEventTPCnv_15.8.0   AOD-15.8.0-full
                  REFERENCE_TAG ${TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrigTruthEventTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${TRIGTRUTHEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
