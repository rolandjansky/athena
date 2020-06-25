################################################################################
# Package: PhysicsElementLinksAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( PhysicsElementLinksAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/AnalysisCommon/ParticleEventAthenaPool
                          PhysicsAnalysis/PhysicsElementLinks )

# Component(s) in the package:
atlas_add_poolcnv_library( PhysicsElementLinksAthenaPoolPoolCnv
                           src/*.cxx
                           FILES PhysicsElementLinks/JetLinks.h PhysicsElementLinks/TauJetLinks.h PhysicsElementLinks/MuonLinks.h PhysicsElementLinks/ElectronLinks.h PhysicsElementLinks/PhotonLinks.h PhysicsElementLinks/TruthParticleLinks.h PhysicsElementLinks/TrackLinks.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib PhysicsElementLinks ParticleEventAthenaPoolLib )

