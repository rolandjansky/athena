################################################################################
# Package: TrkGeometry
################################################################################

# Declare the package name:
atlas_subdir( TrkGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometrySurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Event/EventPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkGeometry
                   src/*.cxx
                   PUBLIC_HEADERS TrkGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometrySurfaces TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives )

atlas_add_dictionary( TrkGeometryDict
                      TrkGeometry/TrkGeometryDict.h
                      TrkGeometry/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometrySurfaces TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters EventPrimitives TrkGeometry )

