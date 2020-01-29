################################################################################
# Package: xAODTauAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTauAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
			  Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
			  Event/xAOD/xAODTracking
                          Event/xAOD/xAODTau
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTauAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h xAODTau/DiTauJetContainer.h xAODTau/DiTauJetAuxContainer.h xAODTau/TauTrackContainer.h xAODTau/TauTrackAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer xAOD::DiTauJetContainer xAOD::DiTauJetAuxContainer xAOD::TauTrackContainer xAOD::TauTrackAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel StoreGateLib AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTracking xAODTau GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTAUATHENAPOOL_REFERENCE_TAG
       xAODTauAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( xAODTauAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTauAthenaPool_20.7.2.2   AOD-20.7.2.2-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTauAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
