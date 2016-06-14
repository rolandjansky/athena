################################################################################
# Package: InDetTrackingGeometryXML
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackingGeometryXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/InDetTrackingGeometryUtils
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkGeometryAlpine )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( InDetTrackingGeometryXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES}
		     AthenaBaseComps AthenaKernel AtlasDetDescr GeoPrimitives StoreGateLib SGtests
		     PRIVATE_LINK_LIBRARIES GaudiKernel InDetIdentifier TrkDetDescrInterfaces TrkDetDescrUtils TrkSurfaces 
		     InDetReadoutGeometry InDetTrackingGeometryUtils TrkDetElementBase TrkGeometry TrkGeometryAlpine )

# Install files from the package:
atlas_install_headers( InDetTrackingGeometryXML )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.xml )

