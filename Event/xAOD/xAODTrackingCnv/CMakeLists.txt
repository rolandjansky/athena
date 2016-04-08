################################################################################
# Package: xAODTrackingCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackingCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventPrimitives
                          GaudiKernel
                          Generators/GeneratorObjects
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( xAODTrackingCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTracking TrkTrack AthenaBaseComps AthenaKernel EventPrimitives GaudiKernel GeneratorObjects MCTruthClassifierLib Particle ParticleTruth TrkLinks TrkParticleBase TrkTruthData VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( xAODTrackingCnv )

