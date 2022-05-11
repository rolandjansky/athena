################################################################################
# Package: DCMathSegmentMaker
################################################################################

# Declare the package name:
atlas_subdir( DCMathSegmentMaker )

# Component(s) in the package:
atlas_add_component( DCMathSegmentMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib GeoPrimitives EventPrimitives GaudiKernel MuonReadoutGeometry 
                                    MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib 
                                    MuonRecToolInterfaces MuonStationIntersectCondLib TrkGeometry TrkSurfaces TrkEventPrimitives TrkParameters 
                                    TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkRoad TrkTrack TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces
                                    TrkDriftCircleMath MuonCondData MuonSegmentMakerToolInterfaces MuonSegmentMakerInterfacesLib FourMomUtils)
