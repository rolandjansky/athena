################################################################################
# Package: VxJetVertex
################################################################################

# Declare the package name:
atlas_subdir( VxJetVertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Control/DataModel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VxJetVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxJetVertex
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES EventPrimitives GaudiKernel TrkEventPrimitives TrkParticleBase TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel )

atlas_add_dictionary( VxJetVertexDict
                      VxJetVertex/VxJetVertexDict.h
                      VxJetVertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives GaudiKernel TrkEventPrimitives TrkParticleBase TrkTrack VxVertex DataModel VxJetVertex )


