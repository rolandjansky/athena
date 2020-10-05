################################################################################
# Package: MuonSegmentOverlapRemovalTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentOverlapRemovalTools )

# Component(s) in the package:
atlas_add_component( MuonSegmentOverlapRemovalTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonClusterizationLib MuonCompetingRIOsOnTrack MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils TrkEventPrimitives MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces )

