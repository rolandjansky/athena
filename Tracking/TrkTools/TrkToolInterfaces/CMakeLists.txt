################################################################################
# Package: TrkToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrkToolInterfaces
                   PUBLIC_HEADERS TrkToolInterfaces
                   LINK_LIBRARIES AthContainers AthLinks EventPrimitives xAODTracking xAODTruth GaudiKernel TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkParameters TrkParticleBase TrkSegment TrkTrack TrkTrackSummary TrkEventUtils CxxUtils )

