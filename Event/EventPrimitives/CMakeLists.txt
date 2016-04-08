################################################################################
# Package: EventPrimitives
################################################################################

# Declare the package name:
atlas_subdir( EventPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( EventPrimitives
                   PUBLIC_HEADERS EventPrimitives
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} )

