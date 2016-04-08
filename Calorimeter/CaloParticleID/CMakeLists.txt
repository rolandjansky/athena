################################################################################
# Package: CaloParticleID
################################################################################

# Declare the package name:
atlas_subdir( CaloParticleID )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/Navigation
                          Generators/GenAnalysisTools/TruthHelper )

# Component(s) in the package:
atlas_add_component( CaloParticleID
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloSimEvent AthenaBaseComps GaudiKernel McParticleEvent CaloDetDescrLib Navigation TruthHelper )

# Install files from the package:
atlas_install_headers( CaloParticleID )

