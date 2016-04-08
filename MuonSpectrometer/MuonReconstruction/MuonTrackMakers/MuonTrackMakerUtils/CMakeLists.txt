################################################################################
# Package: MuonTrackMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackMakerUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Eigen )

# Install files from the package:
atlas_install_headers( MuonTrackMakerUtils )

