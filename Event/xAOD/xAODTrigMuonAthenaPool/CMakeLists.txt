################################################################################
# Package: xAODTrigMuonAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMuonAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigMuon )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMuonAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMuon/L2StandAloneMuonContainer.h xAODTrigMuon/L2StandAloneMuonAuxContainer.h xAODTrigMuon/L2IsoMuonContainer.h xAODTrigMuon/L2IsoMuonAuxContainer.h xAODTrigMuon/L2CombinedMuonContainer.h xAODTrigMuon/L2CombinedMuonAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::L2StandAloneMuonContainer xAOD::L2StandAloneMuonAuxContainer xAOD::L2IsoMuonContainer xAOD::L2IsoMuonAuxContainer xAOD::L2CombinedMuonContainer xAOD::L2CombinedMuonAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMuon )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGMONATHENAPOOL_REFERENCE_TAG
       xAODTrigMuonAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigMuonAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTrigMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTrigMuonAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMONATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
