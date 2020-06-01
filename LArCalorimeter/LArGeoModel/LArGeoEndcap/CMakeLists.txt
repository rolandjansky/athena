################################################################################
# Package: LArGeoEndcap
################################################################################

# Declare the package name:
atlas_subdir( LArGeoEndcap )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          Database/RDBAccessSvc
                          LArCalorimeter/LArGeoModel/LArGeoFcal
                          LArCalorimeter/LArGeoModel/LArGeoHec
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoModel/GeoSpecialShapes
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArGeoModel/LArGeoMiniFcal )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoEndcap
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoEndcap
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} LArGeoFcal LArGeoHec StoreGateLib SGtests CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier GeoModelUtilities GeoSpecialShapes GaudiKernel LArGeoCode LArGeoMiniFcal RDBAccessSvcLib )

