################################################################################
# Package: xAODPFlowAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODPFlowAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODPFlow )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODPFlowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODPFlow/PFOContainer.h xAODPFlow/PFOAuxContainer.h xAODPFlow/TrackCaloClusterContainer.h xAODPFlow/TrackCaloClusterAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::PFOContainer xAOD::PFOAuxContainer xAOD::TrackCaloClusterContainer xAOD::TrackCaloClusterAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODPFlow )




# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODPFLOWATHENAPOOL_REFERENCE_TAG
       xAODPFlowAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODPFlowAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODPFlowAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODPFLOWATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
