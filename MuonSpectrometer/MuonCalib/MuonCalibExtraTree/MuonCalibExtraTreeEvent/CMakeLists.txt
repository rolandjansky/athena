################################################################################
# Package: MuonCalibExtraTreeEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraTreeEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraTreeEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraTreeEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives MuonCalibEventBase MuonCalibExtraUtils MuonCalibIdentifier )

