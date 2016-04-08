################################################################################
# Package: TrkMeasurementBase
################################################################################

# Declare the package name:
atlas_subdir( TrkMeasurementBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          Control/AthContainers )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkMeasurementBase
                   PUBLIC_HEADERS TrkMeasurementBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives )

atlas_add_dictionary( TrkMeasurementBaseDict
                      TrkMeasurementBase/TrkMeasurementBaseDict.h
                      TrkMeasurementBase/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives AthContainers TrkMeasurementBase )

