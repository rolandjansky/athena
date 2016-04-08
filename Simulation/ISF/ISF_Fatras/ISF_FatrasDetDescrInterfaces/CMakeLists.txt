################################################################################
# Package: ISF_FatrasDetDescrInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasDetDescrInterfaces
                   PUBLIC_HEADERS ISF_FatrasDetDescrInterfaces
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel InDetReadoutGeometry )

