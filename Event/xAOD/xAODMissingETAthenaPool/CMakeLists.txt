################################################################################
# Package: xAODMissingETAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODMissingETAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODMissingET
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMissingETAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODMissingET/MissingETContainer.h xAODMissingET/MissingETAuxContainer.h xAODMissingET/MissingETComponentMap.h xAODMissingET/MissingETAuxComponentMap.h xAODMissingET/MissingETAssociationMap.h xAODMissingET/MissingETAuxAssociationMap.h
                           TYPES_WITH_NAMESPACE xAOD::MissingETContainer xAOD::MissingETAuxContainer xAOD::MissingETComponentMap xAOD::MissingETAuxComponentMap xAOD::MissingETAssociationMap xAOD::MissingETAuxAssociationMap
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMissingET GaudiKernel )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODMISSINGETATHENAPOOL_REFERENCE_TAG
       xAODMissingETAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODMissingETAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODMissingETAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMISSINGETATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODMissingETAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODMissingETAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMISSINGETATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
