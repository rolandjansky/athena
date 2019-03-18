################################################################################
# Package: InDetPrepRawData
################################################################################

# Declare the package name:
atlas_subdir( InDetPrepRawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/CxxUtils
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS InDetPrepRawData
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthLinks CxxUtils SGTools GeoPrimitives Identifier EventPrimitives GaudiKernel InDetReadoutGeometry TrkSurfaces TrkEventPrimitives TrkPrepRawData
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( InDetPrepRawDataDict
                      InDetPrepRawData/InDetPrepRawDataDict.h
                      InDetPrepRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks CxxUtils SGTools GeoPrimitives Identifier EventPrimitives GaudiKernel InDetReadoutGeometry TrkSurfaces TrkEventPrimitives TrkPrepRawData InDetPrepRawData EventContainers )

