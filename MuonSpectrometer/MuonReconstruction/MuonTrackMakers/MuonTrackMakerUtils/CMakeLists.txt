# $Id: CMakeLists.txt 770061 2016-08-25 12:52:45Z krasznaa $
################################################################################
# Package: MuonTrackMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackMakerUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CxxUtils
   DetectorDescription/GeoPrimitives
   MuonSpectrometer/MuonIdHelpers
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
   MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_library( MuonTrackMakerUtils
   MuonTrackMakerUtils/*.h
   INTERFACE
   PUBLIC_HEADERS MuonTrackMakerUtils
   LINK_LIBRARIES CxxUtils GeoPrimitives MuonIdHelpersLib
   MuonCompetingRIOsOnTrack MuonRecHelperToolsLib TrkSurfaces
   TrkMeasurementBase TrkParameters TrkTrack )
