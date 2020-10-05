################################################################################
# Package: MuonSegmentMatchingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMatchingTools )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonSegmentMatchingTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonCompetingRIOsOnTrack MuonSegment MuonRecHelperToolsLib TrkDetDescrUtils TrkGeometry TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces MuonSegmentMakerToolInterfaces )

