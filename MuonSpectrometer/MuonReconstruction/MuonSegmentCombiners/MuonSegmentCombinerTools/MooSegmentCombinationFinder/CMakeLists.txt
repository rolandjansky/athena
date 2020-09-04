################################################################################
# Package: MooSegmentCombinationFinder
################################################################################

# Declare the package name:
atlas_subdir( MooSegmentCombinationFinder )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MooSegmentCombinationFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonChamberT0s MuonPattern MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLinearSegmentMakerUtilities MuonSegmentMakerUtils MuonEDM_AssociationObjects TrkSurfaces TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkTruthData TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces CscSegmentMakersLib MuonPatternSegmentMakerLib MuonRecToolInterfaces )

