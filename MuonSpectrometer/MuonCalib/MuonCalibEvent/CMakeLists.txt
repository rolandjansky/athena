################################################################################
# Package: MuonCalibEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibEvent
                   src/MdtCalibHit.cxx
                   PUBLIC_HEADERS MuonCalibEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers GeoPrimitives MuonCalibEventBase MuonPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonCalibITools )

