################################################################################
# Package: MuonTrackFindingEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackFindingEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase )

# Component(s) in the package:
atlas_add_library( MuonTrackFindingEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonTrackFindingEvent
                   LINK_LIBRARIES Identifier EventPrimitives MuonSegment TrkParameters TrkTrack MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES TrkEventPrimitives TrkMeasurementBase )

