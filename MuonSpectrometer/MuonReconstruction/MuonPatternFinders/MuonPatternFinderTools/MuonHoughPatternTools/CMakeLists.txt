################################################################################
# Package: MuonHoughPatternTools
################################################################################

# Declare the package name:
atlas_subdir( MuonHoughPatternTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonHoughPatternToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonHoughPatternTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS 
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MuonDetDescrUtils MuonHoughPatternEvent MuonPattern MuonPrepRawData MuonSegment MuonRecToolInterfaces MuonLayerHough TrkDriftCircleMath MuonIdHelpersLib MuonClusterizationLib MuonRecHelperToolsLib StoreGateLib
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib CxxUtils EventPrimitives xAODMuon xAODTruth MuonReadoutGeometry MuonRIO_OnTrack TrkSurfaces TrkTruthData FourMomUtils)

atlas_add_component( MuonHoughPatternTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps GeoPrimitives GaudiKernel MuonDetDescrUtils MuonIdHelpersLib MuonClusterizationLib MuonHoughPatternEvent MuonPattern MuonPrepRawData MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough TrkDriftCircleMath CxxUtils StoreGateLib EventPrimitives xAODMuon xAODTruth MuonReadoutGeometry MuonRIO_OnTrack TrkSurfaces TrkTruthData MuonHoughPatternToolsLib )

