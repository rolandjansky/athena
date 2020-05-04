################################################################################
# Package: LArGeoCode
################################################################################

# Declare the package name:
atlas_subdir( LArGeoCode )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoCode
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoCode
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GaudiKernel GeoModelInterfaces RDBAccessSvcLib )

