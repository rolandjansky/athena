################################################################################
# Package: LArGeoTBBarrel
################################################################################

# Declare the package name:
atlas_subdir( LArGeoTBBarrel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArGeoBarrel )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoTBBarrel
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBBarrel
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoModelUtilities GaudiKernel LArGeoBarrel )

