################################################################################
# Package: xAODTrigRingerAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigRingerAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigRinger
                          GaudiKernel )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigRingerAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigRinger/TrigRingerRingsContainer.h xAODTrigRinger/TrigRingerRingsAuxContainer.h xAODTrigRinger/TrigRNNOutputContainer.h xAODTrigRinger/TrigRNNOutputAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigRingerRingsContainer xAOD::TrigRingerRingsAuxContainer xAOD::TrigRNNOutputContainer xAOD::TrigRNNOutputAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigRinger GaudiKernel )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGRINGERATHENAPOOL_REFERENCE_TAG
       xAODTrigRingerAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTrigRingerAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigRingerAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGRINGERATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
