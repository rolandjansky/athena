################################################################################
# Package: Particle
################################################################################

# Declare the package name:
atlas_subdir( Particle )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/AthLinks
                          Control/Navigation
                          Event/EventKernel
                          Event/FourMom
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( Particle
                   src/*.cxx
                   PUBLIC_HEADERS Particle
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack TrkTrackSummary VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( ParticleDict
                      Particle/ParticleDict.h
                      Particle/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack TrkTrackSummary VxVertex Particle
                      NAVIGABLES Rec::TrackParticleContainer )

