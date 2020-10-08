################################################################################
# Package: MuonCalibMath
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibMath )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS MathCore MathMore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibMath
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibMath
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} EventPrimitives AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} )

