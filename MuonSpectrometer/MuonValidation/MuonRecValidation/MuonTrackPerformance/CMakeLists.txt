################################################################################
# Package: MuonTrackPerformance
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackPerformance )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTrackPerformance
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps xAODEventInfo GaudiKernel MuonIdHelpersLib MuonStationIndexLib MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces TrkParameters TrkTrack TrkTrackSummary TrkTruthData TrkToolInterfaces StoreGateLib SGtests Identifier EventPrimitives xAODMuon xAODTracking xAODTruth MuonReadoutGeometry MuonClusterizationLib MuonCompetingRIOsOnTrack MuonLayerEvent MuonPrepRawData MuonRIO_OnTrack MuonLayerHough MuonRecValidationNtuples MuonCombinedEvent TrkMeasurementBase TrkPrepRawData TrkExInterfaces TrackRecordLib MuonSegmentMakerToolInterfaces MuonSegmentTaggerToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonTrackPerformance )
atlas_install_joboptions( share/*.py )

