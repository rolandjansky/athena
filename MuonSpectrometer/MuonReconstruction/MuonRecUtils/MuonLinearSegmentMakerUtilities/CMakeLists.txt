################################################################################
# Package: MuonLinearSegmentMakerUtilities
################################################################################

# Declare the package name:
atlas_subdir( MuonLinearSegmentMakerUtilities )

# External dependencies:
find_package( Eigen )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonLinearSegmentMakerUtilities
                   src/*.cxx
                   PUBLIC_HEADERS MuonLinearSegmentMakerUtilities
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${GSL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers GeoPrimitives GaudiKernel MuonStationIndexLib
                   PRIVATE_LINK_LIBRARIES ${GSL_LIBRARIES} EventPrimitives MuonReadoutGeometry MuonSegment TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack )

