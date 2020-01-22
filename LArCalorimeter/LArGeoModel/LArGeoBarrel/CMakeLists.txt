################################################################################
# Package: LArGeoBarrel
################################################################################

# Declare the package name:
atlas_subdir( LArGeoBarrel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          PRIVATE
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoBarrel
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoBarrel
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} LArGeoCode StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GeoModelUtilities GaudiKernel LArReadoutGeometry )

