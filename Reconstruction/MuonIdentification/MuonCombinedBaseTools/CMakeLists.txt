################################################################################
# Package: MuonCombinedBaseTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedBaseTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCombinedBaseTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib CaloEvent AthLinks AthenaBaseComps Identifier EventPrimitives FourMomUtils xAODCaloEvent xAODEventInfo xAODMuon xAODTracking xAODTruth GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils MuonSelectorToolsLib ICaloTrkMuIdTools MuidInterfaces MuonCombinedEvent MuonCombinedToolInterfaces muonEvent MuidEvent ParticleTruth RecoToolInterfaces TrackToCaloLib TrkGeometry TrkSurfaces TrkCaloExtension TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkParameters TrkParametersIdentificationHelpers TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces TrkToolInterfaces BeamSpotConditionsData MagFieldElements MagFieldConditions xAODMuonCnvLib MuonSegmentTaggerToolInterfaces )

