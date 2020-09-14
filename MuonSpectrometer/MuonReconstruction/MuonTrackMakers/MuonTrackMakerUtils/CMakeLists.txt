################################################################################
# Package: MuonTrackMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackMakerUtils )

# Component(s) in the package:
atlas_add_library( MuonTrackMakerUtils
   MuonTrackMakerUtils/*.h
   INTERFACE
   PUBLIC_HEADERS MuonTrackMakerUtils
   LINK_LIBRARIES CxxUtils GeoPrimitives MuonIdHelpersLib
   MuonCompetingRIOsOnTrack MuonRecHelperToolsLib TrkSurfaces
   TrkMeasurementBase TrkParameters TrkTrack )
