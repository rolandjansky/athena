################################################################################
# Package: TrkSurfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkSurfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParametersBase
                          PRIVATE
                          Control/CxxUtils )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkSurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkSurfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives Identifier EventPrimitives GaudiKernel TrkDetDescrUtils TrkDetElementBase TrkEventPrimitives TrkParametersBase
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils )

atlas_add_dictionary( TrkSurfacesDict
                      TrkSurfaces/TrkSurfacesDict.h
                      TrkSurfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives Identifier EventPrimitives GaudiKernel TrkDetDescrUtils TrkDetElementBase TrkEventPrimitives TrkParametersBase CxxUtils TrkSurfaces )

