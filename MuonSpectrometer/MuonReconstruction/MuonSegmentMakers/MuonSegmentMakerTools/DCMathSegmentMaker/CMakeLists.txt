################################################################################
# Package: DCMathSegmentMaker
################################################################################

# Declare the package name:
atlas_subdir( DCMathSegmentMaker )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( DCMathSegmentMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonStationIntersectSvcLib TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkRoad TrkTrack TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces TrkDriftCircleMath MuonCondData MuonSegmentMakerToolInterfaces MuonSegmentMakerInterfacesLib )
