################################################################################
# Package: MuonCombinedTrackFindingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTrackFindingTools )

# Component(s) in the package:
atlas_add_component( MuonCombinedTrackFindingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps EventPrimitives xAODTracking xAODTruth MdtCalibData MdtCalibSvcLib MuonDetDescrUtils MuonIdHelpersLib MuonStationIndexLib MuonClusterizationLib MuonHoughPatternToolsLib MuonCompetingRIOsOnTrack MuonLayerEvent MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough MuidInterfaces MuonCombinedEvent MuonCombinedToolInterfaces MuonPrepRawDataProviderToolsLib TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces TrkDriftCircleMath MuonRecHelperToolsLib MuonSegmentMakerToolInterfaces MuonSegmentTaggerToolInterfaces )

