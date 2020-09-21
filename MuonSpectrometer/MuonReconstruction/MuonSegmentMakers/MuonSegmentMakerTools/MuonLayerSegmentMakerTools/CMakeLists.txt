################################################################################
# Package: MuonLayerSegmentMakerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonLayerSegmentMakerTools )

# Component(s) in the package:
atlas_add_component( MuonLayerSegmentMakerTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps EventPrimitives MuonPrepRawDataProviderToolsLib MuonDetDescrUtils MuonIdHelpersLib MuonHoughPatternToolsLib MuonLayerEvent MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough TrkSegment MuonSegmentMakerToolInterfaces CscSegmentMakersLib )

