################################################################################
# Package: MuonTrackSteeringTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackSteeringTools )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonTrackSteeringTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GeoPrimitives Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonStationIndexLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils TrkDetDescrUtils TrkGeometry TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkToolInterfaces TrkDriftCircleMath MagFieldElements MagFieldConditions MuonSegmentMakerToolInterfaces MuonTrackMakerUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )

