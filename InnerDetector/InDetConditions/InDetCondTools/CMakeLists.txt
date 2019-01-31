################################################################################
# Package: InDetCondTools
################################################################################

# Declare the package name:
atlas_subdir( InDetCondTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( InDetCondTools
                   PUBLIC_HEADERS InDetCondTools
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel )

