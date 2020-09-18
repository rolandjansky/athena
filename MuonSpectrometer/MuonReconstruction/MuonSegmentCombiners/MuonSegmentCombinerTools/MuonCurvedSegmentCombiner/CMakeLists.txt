################################################################################
# Package: MuonCurvedSegmentCombiner
################################################################################

# Declare the package name:
atlas_subdir( MuonCurvedSegmentCombiner )

# Component(s) in the package:
atlas_add_component( MuonCurvedSegmentCombiner
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonEDM_AssociationObjects StoreGateLib SGtests Identifier MuonReadoutGeometry MuonIdHelpersLib MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonCurvedSegmentCombiner )

