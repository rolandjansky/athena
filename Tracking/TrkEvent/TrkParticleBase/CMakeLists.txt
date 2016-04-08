################################################################################
# Package: TrkParticleBase
################################################################################

# Declare the package name:
atlas_subdir( TrkParticleBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrackSummary )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkParticleBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkParticleBase
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools TrkEventPrimitives TrkParameters TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrkTrackSummary )

atlas_add_dictionary( TrkParticleBaseDict
                      TrkParticleBase/TrkParticleBaseDict.h
                      TrkParticleBase/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools TrkEventPrimitives TrkParameters TrkTrack VxVertex GaudiKernel TrkTrackSummary TrkParticleBase
                      ELEMENT_LINKS DataVector<Trk::TrackParticleBase> )

