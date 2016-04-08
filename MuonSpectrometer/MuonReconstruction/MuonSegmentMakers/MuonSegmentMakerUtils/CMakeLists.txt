################################################################################
# Package: MuonSegmentMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( MuonSegmentMakerUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegmentMakerUtils
                   LINK_LIBRARIES MuonSegment TrkEventPrimitives MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES MuonCompetingRIOsOnTrack MuonRIO_OnTrack TrkRIO_OnTrack )

