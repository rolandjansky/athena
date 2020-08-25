################################################################################
# Package: InDetGeoModelUtils
################################################################################

# Declare the package name:
atlas_subdir( InDetGeoModelUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/RDBAccessSvc
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
			  DetectorDescription/GeoPrimitives
                          DetectorDescription/GeometryDBSvc )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( InDetGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS InDetGeoModelUtils
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel CxxUtils GaudiKernel StoreGateLib SGtests RDBAccessSvcLib GeoPrimitives
                   PRIVATE_LINK_LIBRARIES GeoModelUtilities GeometryDBSvcLib )

