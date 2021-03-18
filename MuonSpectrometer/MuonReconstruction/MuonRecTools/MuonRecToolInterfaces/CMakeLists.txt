################################################################################
# Package: MuonRecToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonRecToolInterfaces )

# Component(s) in the package:
atlas_add_library( MuonRecToolInterfaces
                   PUBLIC_HEADERS MuonRecToolInterfaces
                   LINK_LIBRARIES GeoPrimitives Identifier EventPrimitives xAODTracking GeneratorObjects GaudiKernel MuonClusterizationLib MuonLayerEvent MuonPattern MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonEDM_AssociationObjects MuonLayerHough MuonSimData MuonCombinedEvent TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkTrack TrkTruthData TrkToolInterfaces TrkDriftCircleMath MuonStationIndexLib TrkCompetingRIOsOnTrackToolLib )

