# $Id: CMakeLists.txt 770571 2016-08-29 11:37:46Z krasznaa $
################################################################################
# Package: xAODTruthAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTruthAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthContainers
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODTruth )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTruthAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTruth/TruthParticleContainer.h
   xAODTruth/TruthParticleAuxContainer.h
   xAODTruth/TruthVertexContainer.h
   xAODTruth/TruthVertexAuxContainer.h
   xAODTruth/TruthEventContainer.h
   xAODTruth/TruthEventAuxContainer.h
   xAODTruth/TruthPileupEventContainer.h
   xAODTruth/TruthPileupEventAuxContainer.h
   xAODTruth/TruthMetaDataContainer.h
   xAODTruth/TruthMetaDataAuxContainer.h
   TYPES_WITH_NAMESPACE
   xAOD::TruthParticleContainer xAOD::TruthParticleAuxContainer
   xAOD::TruthVertexContainer xAOD::TruthVertexAuxContainer
   xAOD::TruthEventContainer xAOD::TruthEventAuxContainer
   xAOD::TruthPileupEventContainer xAOD::TruthPileupEventAuxContainer
   xAOD::TruthMetaDataContainer xAOD::TruthMetaDataAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODTruth )



# Set up (a) test(s) for the converter(s):
# Don't run tests in AthSimulation, due to problems reading recoMuonLink.
if( NOT SIMULATIONBASE )
  if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
     set( AthenaPoolUtilitiesTest_DIR
        ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
  endif()
  find_package( AthenaPoolUtilitiesTest )

  if( ATHENAPOOLUTILITIESTEST_FOUND )
    set( XAODTRUTHATHENAPOOL_REFERENCE_TAG
         xAODTruthAthenaPoolReference-01-00-00 )
    run_tpcnv_legacy_test( xAODTruthAthenaPool_21.0.79   AOD-21.0.79-full
                     REQUIRED_LIBRARIES xAODTruthAthenaPoolPoolCnv
                     REFERENCE_TAG ${XAODTRUTHATHENAPOOL_REFERENCE_TAG} )
  else()
     message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
  endif()   
endif()   
                         
