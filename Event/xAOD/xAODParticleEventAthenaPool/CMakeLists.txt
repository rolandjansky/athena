################################################################################
# Package: xAODParticleEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODParticleEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODParticleEvent )

atlas_install_joboptions( share/*.py )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODParticleEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODParticleEvent/ParticleContainer.h xAODParticleEvent/ParticleAuxContainer.h xAODParticleEvent/CompositeParticleContainer.h xAODParticleEvent/CompositeParticleAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::ParticleContainer xAOD::ParticleAuxContainer xAOD::CompositeParticleContainer xAOD::CompositeParticleAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODParticleEvent )



# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODPARTICLEEVENTATHENAPOOL_REFERENCE_TAG
       xAODParticleEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODParticleAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODParticleEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODPARTICLEEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
                         
