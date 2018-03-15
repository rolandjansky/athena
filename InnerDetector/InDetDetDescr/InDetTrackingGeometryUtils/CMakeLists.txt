################################################################################
# Package: InDetTrackingGeometryUtils
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackingGeometryUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetTrackingGeometryUtils
                   src/*.cxx
                   PUBLIC_HEADERS InDetTrackingGeometryUtils
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES}
		   GeoPrimitives InDetIdentifier TrkDetDescrUtils TrkGeometry 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier InDetReadoutGeometry TrkSurfaces StoreGateLib SGtests )

