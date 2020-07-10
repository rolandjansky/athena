################################################################################
# Package: InDetTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          AtlasGeometryCommon/BeamPipeGeoModel
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          Tracking/TrkDetDescr/TrkDetDescrGeoModelCnv
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( InDetTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps CxxUtils GeoPrimitives GaudiKernel InDetIdentifier TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry BeamPipeGeoModelLib StoreGateLib SGtests Identifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry TrkDetDescrGeoModelCnv TrkSurfaces TrkVolumes SubDetectorEnvelopesLib )

# Install files from the package:
atlas_install_headers( InDetTrackingGeometry )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

