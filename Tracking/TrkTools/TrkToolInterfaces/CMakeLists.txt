################################################################################
# Package: TrkToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkEventUtils )

# Component(s) in the package:
atlas_add_library( TrkToolInterfaces
                   PUBLIC_HEADERS TrkToolInterfaces
                   LINK_LIBRARIES AthContainers AthLinks EventPrimitives xAODTracking xAODTruth GaudiKernel TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkParameters TrkParticleBase TrkSegment TrkTrack TrkTrackSummary TrkEventUtils )

