################################################################################
# Package: SCT_SLHC_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( SCT_SLHC_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc
			  DetectorDescription/GeoPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_component( SCT_SLHC_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils InDetReadoutGeometry SGTools StoreGateLib SGtests InDetIdentifier )

