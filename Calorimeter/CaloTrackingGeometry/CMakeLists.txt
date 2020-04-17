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

atlas_add_library( CaloTrackingGeometryLib
                   INTERFACE
                   PUBLIC_HEADERS CaloTrackingGeometry
                   LINK_LIBRARIES GaudiKernel GeoPrimitives CaloIdentifier CaloGeoHelpers CaloDetDescr )

# Component(s) in the package:
atlas_add_component( CaloTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces StoreGateLib SGtests TileDetDescr TrkDetDescrUtils TrkGeometry TrkGeometrySurfaces TrkSurfaces TrkVolumes SubDetectorEnvelopesLib )

atlas_add_dictionary( CaloTrackingGeometryDict
                      CaloTrackingGeometry/CaloTrackingGeometryDict.h
                      CaloTrackingGeometry/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces StoreGateLib SGtests TileDetDescr TrkDetDescrUtils TrkGeometry TrkGeometrySurfaces TrkSurfaces TrkVolumes )

# Install files from the package:
atlas_install_headers( CaloTrackingGeometry )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
