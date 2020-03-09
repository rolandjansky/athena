################################################################################
# Package: xAODTrigMissingETAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMissingETAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMissingETAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMissingET/TrigMissingETContainer.h xAODTrigMissingET/TrigMissingETAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigMissingET xAOD::TrigMissingETComponent xAOD::TrigMissingETContainer xAOD::TrigMissingETAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMissingET GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGMISSINGETATHENAPOOL_REFERENCE_TAG
       xAODTrigMissingETAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( xAODTrigMissingETAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigMissingETAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMISSINGETATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
