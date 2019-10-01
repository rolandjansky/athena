################################################################################
# Package: xAODJetAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODJetAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODJet )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_install_joboptions( share/*.py )

set( poolcnv_files )
set( poolcnv_types )

if( NOT SIMULATIONBASE )
   set( poolcnv_files xAODJet/JetContainer.h xAODJet/JetAuxContainer.h xAODJet/JetTrigAuxContainer.h)
   set( poolcnv_types xAOD::JetContainer xAOD::JetAuxContainer xAOD::JetTrigAuxContainer)
else()
   set( poolcnv_files xAODJet/JetContainer.h xAODJet/JetAuxContainer.h)
   set( poolcnv_types xAOD::JetContainer xAOD::JetAuxContainer)
endif()

# Component(s) in the package:
atlas_add_poolcnv_library( xAODJetAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ${poolcnv_files}
                           TYPES_WITH_NAMESPACE ${poolcnv_types}
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODJet )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODJETATHENAPOOL_REFERENCE_TAG
       xAODJetAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODJetAthenaPool_20.7.2.2   AOD-20.7.2.2-full
                   REQUIRED_LIBRARIES xAODJetAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODJETATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODJetAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODJetAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODJETATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
                         
