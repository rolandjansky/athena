################################################################################
# Package: TrkTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkTruthData
                          PRIVATE
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          Generators/GeneratorObjects
                          PhysicsAnalysis/MCTruthClassifier
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTruthData xAODCore xAODTracking GeneratorObjects MCTruthClassifierLib TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkTruthAlgs )

