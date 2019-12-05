################################################################################
# Package: xAODEventShapeAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEventShapeAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventShape )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventShapeAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEventShape/EventShape.h xAODEventShape/EventShapeAuxInfo.h
                           TYPES_WITH_NAMESPACE xAOD::EventShape xAOD::EventShapeAuxInfo
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEventShape )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODEVENTSHAPEATHENAPOOL_REFERENCE_TAG
       xAODEventShapeAthenaPoolReference-01-00-01 )
  run_tpcnv_test( xAODEventShapeAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODEventShapeAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODEVENTSHAPEATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
                         
