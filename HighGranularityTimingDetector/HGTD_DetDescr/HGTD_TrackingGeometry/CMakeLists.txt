################################################################################
# Package: HGTD_TrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( HGTD_TrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelKernel
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_ReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HGTD_TrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     PUBLIC_HEADERS HGTD_TrackingGeometry
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES}  AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry StoreGateLib SGtests GeoModelKernel HGTD_Identifier HGTD_ReadoutGeometry TrkSurfaces)

# Install files from the package:
atlas_install_headers( HGTD_TrackingGeometry )
atlas_install_python_modules( python/*.py )
