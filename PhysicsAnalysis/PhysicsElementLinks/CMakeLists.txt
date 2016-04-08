################################################################################
# Package: PhysicsElementLinks
################################################################################

# Declare the package name:
atlas_subdir( PhysicsElementLinks )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tools/AtlasDoxygen )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PhysicsElementLinks
                   src/*.cxx
                   PUBLIC_HEADERS PhysicsElementLinks
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools ParticleEvent McParticleEvent JetEvent muonEvent Particle egammaEvent tauEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( PhysicsElementLinksDict
                      PhysicsElementLinks/PhysicsElementLinksDict.h
                      PhysicsElementLinks/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools ParticleEvent McParticleEvent JetEvent muonEvent Particle egammaEvent tauEvent PhysicsElementLinks )

