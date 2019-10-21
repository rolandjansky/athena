################################################################################
# Package: xAODTrigCaloAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigCaloAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTrigCalo
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigCaloAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigCalo/TrigCaloClusterContainer.h xAODTrigCalo/TrigCaloClusterAuxContainer.h xAODTrigCalo/TrigEMClusterContainer.h xAODTrigCalo/TrigEMClusterAuxContainer.h xAODTrigCalo/CaloClusterTrigAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigCaloClusterContainer xAOD::TrigCaloClusterAuxContainer xAOD::TrigEMClusterContainer xAOD::TrigEMClusterAuxContainer xAOD::CaloClusterTrigAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloEvent xAODTrigCalo GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGCALOATHENAPOOL_REFERENCE_TAG
       xAODTrigCaloAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigCaloAthenaPool_20.0.0.3   AOD-20.0.0.3-full
                   REQUIRED_LIBRARIES xAODTrigCaloAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGCALOATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTrigCaloAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigCaloAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGCALOATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
