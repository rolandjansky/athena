################################################################################
# Package: MuonSegmentSelectionTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentSelectionTools )

# Component(s) in the package:
atlas_add_component( MuonSegmentSelectionTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonSegmentMakerToolInterfaces )

