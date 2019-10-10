################################################################################
# Package: xAODBTaggingAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODTracking
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODBTaggingAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODBTagging/BTaggingContainer.h xAODBTagging/BTaggingAuxContainer.h xAODBTagging/BTaggingTrigAuxContainer.h xAODBTagging/BTagVertexContainer.h xAODBTagging/BTagVertexAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::BTaggingContainer xAOD::BTaggingAuxContainer xAOD::BTaggingTrigAuxContainer xAOD::BTagVertexContainer xAOD::BTagVertexAuxContainer
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODBTagging xAODTracking GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODBTAGGINGATHENAPOOL_REFERENCE_TAG
       xAODBTaggingAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODBTaggingAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODBTaggingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODBTAGGINGATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
