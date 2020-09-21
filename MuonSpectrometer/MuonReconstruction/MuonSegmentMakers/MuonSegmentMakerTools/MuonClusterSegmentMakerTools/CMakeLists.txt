################################################################################
# Package: MuonClusterSegmentMakerTools
################################################################################

# Declare the package name:
atlas_subdir( MuonClusterSegmentMakerTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonClusterSegmentMakerTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps MuonPrepRawDataProviderToolsLib MuonDetDescrUtils MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLinearSegmentMakerUtilities MuonSegmentMakerUtils TrkParameters TrkTrack TrkTruthData TrkFitterInterfaces TrkToolInterfaces MuonSegmentMakerToolInterfaces )

