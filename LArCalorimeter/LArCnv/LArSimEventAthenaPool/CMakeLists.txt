################################################################################
# Package: LArSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArSimEventTPCnv
                          LArCalorimeter/LArSimEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( LArSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArSimEvent/LArHitContainer.h LArSimEvent/LArHitFloatContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel LArSimEventTPCnv LArSimEvent )

# Install files from the package:
atlas_install_headers( LArSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( LARSIMEVENTATHENAPOOL_REFERENCE_TAG
       LArSimEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( LArSimEventTPCnv_HITS   HITS.04919495._000416
                  REFERENCE_TAG ${LARSIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
