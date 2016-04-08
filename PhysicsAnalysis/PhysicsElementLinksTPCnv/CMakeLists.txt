################################################################################
# Package: PhysicsElementLinksTPCnv
################################################################################

# Declare the package name:
atlas_subdir( PhysicsElementLinksTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/AnalysisCommon/ParticleEventTPCnv
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent )

# Component(s) in the package:
atlas_add_library( PhysicsElementLinksTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS PhysicsElementLinksTPCnv
                   LINK_LIBRARIES ParticleEventTPCnv McParticleEvent JetEvent muonEvent Particle egammaEvent tauEvent )

