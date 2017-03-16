################################################################################
# Package: EventBookkeeperAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventBookkeeperMetaData
                          Event/EventBookkeeperTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( EventBookkeeperAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventBookkeeperMetaData/EventBookkeeperCollection.h EventBookkeeperMetaData/EventBookkeeper.h EventBookkeeperMetaData/SkimDecisionCollection.h EventBookkeeperMetaData/SkimDecision.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib EventBookkeeperMetaData EventBookkeeperTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG
       EventBookkeeperAthenaPoolReference-01-00-00 )
  run_tpcnv_test( EventBookkeeperTPCnv_16.0.0   AOD-16.0.0-full
                  REFERENCE_TAG ${EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( EventBookkeeperTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${EVENTBOOKKEEPERATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
