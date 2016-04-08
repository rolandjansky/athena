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

# Component(s) in the package:
atlas_add_poolcnv_library( xAODParticleEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODParticleEvent/ParticleContainer.h xAODParticleEvent/ParticleAuxContainer.h xAODParticleEvent/CompositeParticleContainer.h xAODParticleEvent/CompositeParticleAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::ParticleContainer xAOD::ParticleAuxContainer xAOD::CompositeParticleContainer xAOD::CompositeParticleAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODParticleEvent )

