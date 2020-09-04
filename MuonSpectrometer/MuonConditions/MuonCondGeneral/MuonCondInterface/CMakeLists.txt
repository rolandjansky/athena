################################################################################
# Package: MuonCondInterface
################################################################################

# Declare the package name:
atlas_subdir( MuonCondInterface )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCondInterface
                   PUBLIC_HEADERS MuonCondInterface
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel MuonAlignmentData StoreGateLib SGtests )

