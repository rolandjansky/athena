################################################################################
# Package: MuonPrepRawData
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventContainers
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPrepRawData
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS MuonPrepRawData
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks SGTools GeoPrimitives Identifier EventPrimitives MuonReadoutGeometry TrkSurfaces TrkPrepRawData EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( MuonPrepRawDataDict
                      MuonPrepRawData/MuonPrepDataDict.h
                      MuonPrepRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthLinks SGTools GeoPrimitives Identifier EventPrimitives MuonReadoutGeometry TrkSurfaces TrkPrepRawData GaudiKernel MuonPrepRawData EventContainers)

