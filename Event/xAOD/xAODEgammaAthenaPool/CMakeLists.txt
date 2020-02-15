################################################################################
# Package: xAODEgammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEgammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEgamma
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEgammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEgamma/ElectronContainer.h xAODEgamma/ElectronAuxContainer.h xAODEgamma/PhotonContainer.h xAODEgamma/PhotonAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::ElectronContainer xAOD::ElectronAuxContainer xAOD::PhotonContainer xAOD::PhotonAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEgamma GaudiKernel )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODEGAMMAATHENAPOOL_REFERENCE_TAG
       xAODEgammaAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( xAODEgammaAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODEGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODEgammaAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODEgammaAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODEGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
