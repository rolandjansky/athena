################################################################################
# Package: CaloTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( CaloTrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          PRIVATE
                          Control/StoreGate
                          TileCalorimeter/TileDetDescr
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkGeometrySurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces StoreGateLib SGtests TileDetDescr TrkDetDescrUtils TrkGeometry TrkGeometrySurfaces TrkSurfaces TrkVolumes )

atlas_add_dictionary( CaloTrackingGeometryDict
                      CaloTrackingGeometry/CaloTrackingGeometryDict.h
                      CaloTrackingGeometry/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces StoreGateLib SGtests TileDetDescr TrkDetDescrUtils TrkGeometry TrkGeometrySurfaces TrkSurfaces TrkVolumes )

# Install files from the package:
atlas_install_headers( CaloTrackingGeometry )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

