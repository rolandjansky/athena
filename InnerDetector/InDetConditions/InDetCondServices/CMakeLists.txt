################################################################################
# Package: InDetCondServices
################################################################################

# Declare the package name:
atlas_subdir( InDetCondServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( InDetCondServices
                   PUBLIC_HEADERS InDetCondServices
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel )

