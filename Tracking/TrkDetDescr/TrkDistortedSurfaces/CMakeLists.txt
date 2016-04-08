################################################################################
# Package: TrkDistortedSurfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkDistortedSurfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkDistortedSurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkDistortedSurfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkSurfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrkEventPrimitives )

atlas_add_dictionary( TrkDistortedSurfacesDict
                      TrkDistortedSurfaces/TrkDistortedSurfacesDict.h
                      TrkDistortedSurfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkSurfaces TrkEventPrimitives TrkDistortedSurfaces )

