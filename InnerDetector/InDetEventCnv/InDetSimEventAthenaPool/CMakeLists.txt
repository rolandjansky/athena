################################################################################
# Package: InDetSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          InnerDetector/InDetEventCnv/InDetSimEventTPCnv
                          InnerDetector/InDetSimEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES InDetSimEvent/SiHitCollection.h InDetSimEvent/TRTUncompressedHitCollection.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetSimEventTPCnv InDetSimEvent )

atlas_add_dictionary( InDetSimEventAthenaPoolCnvDict
                      InDetSimEventAthenaPool/InDetSimEventAthenaPoolCnvDict.h
                      InDetSimEventAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetSimEventTPCnv InDetSimEvent )

# Install files from the package:
atlas_install_headers( InDetSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( INDETSIMEVENTATHENAPOOL_REFERENCE_TAG
       InDetSimEventAthenaPoolReference-02-00-00 )
  run_tpcnv_test( InDetSimEventTPCnv_HITS   HITS.04919495._000416
                   REFERENCE_TAG ${INDETSIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
