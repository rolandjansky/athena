################################################################################
# Package: ISF_FatrasInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Simulation/ISF/ISF_Core/ISF_Event
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( ISF_FatrasInterfaces
                   PUBLIC_HEADERS ISF_FatrasInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel GeoPrimitives GaudiKernel ISF_Event TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils )

