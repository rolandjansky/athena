################################################################################
# Package: TrkCaloClusterROI
################################################################################

# Declare the package name:
atlas_subdir( TrkCaloClusterROI )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkCaloClusterROI
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloClusterROI
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrkSurfaces TrkEventPrimitives )

