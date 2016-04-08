################################################################################
# Package: TrackParticleSlimming
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleSlimming )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_library( TrackParticleSlimming
                   src/*.cxx
                   PUBLIC_HEADERS TrackParticleSlimming
                   LINK_LIBRARIES AthenaKernel Particle TrkEventPrimitives TrkParametersBase TrkTrack TrkTrackSummary )

