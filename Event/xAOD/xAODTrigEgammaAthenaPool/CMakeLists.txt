################################################################################
# Package: xAODTrigEgammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigEgammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTrigEgamma
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigEgammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigEgamma/TrigPhotonContainer.h xAODTrigEgamma/TrigPhotonAuxContainer.h xAODTrigEgamma/TrigElectronContainer.h xAODTrigEgamma/TrigElectronAuxContainer.h xAODTrigEgamma/ElectronTrigAuxContainer.h xAODTrigEgamma/PhotonTrigAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigPhotonContainer xAOD::TrigElectronContainer xAOD::ElectronTrigAuxContainer xAOD::PhotonTrigAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEgamma xAODTrigEgamma GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG
       xAODTrigEgammaAthenaPoolReference-01-00-00 )
  #run_tpcnv_test( xAODTrigEgammaAthenaPool_master-20190911   AOD-master-20190911-full
  #                 REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
  #                 REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
  #run_tpcnv_test( xAODTrigEgammaAthenaPool_20.1.7.2   AOD-20.1.7.2-full
  #                 REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
  #                 REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
  #run_tpcnv_test( xAODTrigEgammaAthenaPool_20.7.2.2   AOD-20.7.2.2-full
  #                 REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
  #                 REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
  #run_tpcnv_test( xAODTrigEgammaAthenaPool_20.7.9.9   AOD-20.7.9.9-full
  #                 REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
  #                 REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTrigEgammaAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGEGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
