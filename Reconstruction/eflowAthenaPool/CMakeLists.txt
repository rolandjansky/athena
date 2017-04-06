################################################################################
# Package: eflowAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( eflowAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          Reconstruction/eflowEvent
                          Reconstruction/eflowEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( eflowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES eflowEvent/eflowObjectContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP eflowEvent eflowEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( EFLOWATHENAPOOL_REFERENCE_TAG
       eflowAthenaPoolReference-01-00-00 )
  run_tpcnv_test( eflowEventTPCnv_17.5.0   AOD-17.5.0-full
                  REFERENCE_TAG ${EFLOWATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( eflowEventTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${EFLOWATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
