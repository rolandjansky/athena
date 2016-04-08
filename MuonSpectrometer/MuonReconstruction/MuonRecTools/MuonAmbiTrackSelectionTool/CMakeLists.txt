################################################################################
# Package: MuonAmbiTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( MuonAmbiTrackSelectionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/DataModel
                          Control/StoreGate
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( MuonAmbiTrackSelectionTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkToolInterfaces DataModel StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib TrkCompetingRIOsOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( MuonAmbiTrackSelectionTool )

