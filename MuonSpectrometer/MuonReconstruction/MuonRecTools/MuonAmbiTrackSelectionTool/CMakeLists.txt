################################################################################
# Package: MuonAmbiTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( MuonAmbiTrackSelectionTool )

# Component(s) in the package:
atlas_add_component( MuonAmbiTrackSelectionTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkToolInterfaces AthContainers StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib TrkCompetingRIOsOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( MuonAmbiTrackSelectionTool )

