################################################################################
# Package: xAODCaloEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCaloEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODCaloEvent/CaloClusterContainer.h xAODCaloEvent/CaloClusterAuxContainer.h xAODCaloEvent/CaloTowerContainer.h xAODCaloEvent/CaloTowerAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::CaloClusterContainer xAOD::CaloClusterAuxContainer xAOD::CaloTowerContainer xAOD::CaloTowerAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloEvent GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODCALOEVENTATHENAPOOL_REFERENCE_TAG
       xAODCaloEventAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( xAODCaloEventAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODCaloEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODCALOEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODCaloEventAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODCaloEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODCALOEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
