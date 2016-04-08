################################################################################
# Package: TrkRoad
################################################################################

# Declare the package name:
atlas_subdir( TrkRoad )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkRoad
                   src/*.cxx
                   PUBLIC_HEADERS TrkRoad
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

