################################################################################
# Package: MuonTrackFinderTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackFinderTools )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTrackFinderTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives IRegionSelector Identifier RoiDescriptor EventPrimitives GaudiKernel MuonAlignErrorBase MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonStationIntersectSvcLib MuonSegmentMakerUtils MuidInterfaces TrkFitterInterfaces TrkDetDescrInterfaces TrkDetElementBase TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkRoad TrkTrack TrkSegment TrkTrackSummary TrkExInterfaces TrkToolInterfaces TrkDriftCircleMath MuonCondData MagFieldElements MagFieldConditions MuonSegmentMakerInterfacesLib MuonTrackMakerUtils )

