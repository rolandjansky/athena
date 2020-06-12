################################################################################
# Package: ISF_FatrasInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( ISF_FatrasInterfaces
                   ISF_FatrasInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_FatrasInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers GeoPrimitives GaudiKernel ISF_Event TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils )

