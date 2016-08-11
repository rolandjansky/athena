################################################################################
# Package: MagFieldInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MagFieldInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MagFieldInterfaces
                   PUBLIC_HEADERS MagFieldInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel )

