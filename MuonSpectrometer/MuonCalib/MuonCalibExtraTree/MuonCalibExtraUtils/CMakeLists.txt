################################################################################
# Package: MuonCalibExtraUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraUtils
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives MuonCalibIdentifier )

