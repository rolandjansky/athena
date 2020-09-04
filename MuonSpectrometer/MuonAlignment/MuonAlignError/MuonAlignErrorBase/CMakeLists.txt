################################################################################
# Package: MuonAlignErrorBase
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignErrorBase )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonAlignErrorBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonAlignErrorBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives )

