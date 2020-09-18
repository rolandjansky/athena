################################################################################
# Package: MuonSegmentMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerUtils )

# Component(s) in the package:
atlas_add_library( MuonSegmentMakerUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegmentMakerUtils
                   LINK_LIBRARIES MuonSegment TrkEventPrimitives MuonStationIndexLib
                   PRIVATE_LINK_LIBRARIES MuonCompetingRIOsOnTrack MuonRIO_OnTrack TrkRIO_OnTrack )

