################################################################################
# Package: ParticleEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ParticleEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities )

# Component(s) in the package:
atlas_add_poolcnv_library( ParticleEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ParticleEvent/ParticleBaseContainer.h ParticleEvent/NeutrinoContainer.h ParticleEvent/CompositeParticleContainer.h ParticleEvent/ParticleShallowCloneContainer.h ParticleEvent/SelectedParticles.h ParticleEvent/TrackParticleAssocs.h ParticleEvent/INav4MomToTrackParticleAssocs.h
                           LINK_LIBRARIES StoreGateLib SGtests GaudiKernel ParticleEvent ParticleEventTPCnv AthenaPoolCnvSvcLib AthenaPoolUtilities )

# Install files from the package:
atlas_install_headers( ParticleEventAthenaPool )

