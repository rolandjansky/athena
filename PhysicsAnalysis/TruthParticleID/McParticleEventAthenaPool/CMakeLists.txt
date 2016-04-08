################################################################################
# Package: McParticleEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( McParticleEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEventTPCnv
                          PhysicsAnalysis/TruthParticleID/McParticleKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( McParticleEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES McParticleEvent/TruthParticleContainer.h McParticleEvent/TruthEtIsolationsContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib McParticleEvent McParticleEventTPCnv McParticleKernel )

# Install files from the package:
atlas_install_headers( McParticleEventAthenaPool )

