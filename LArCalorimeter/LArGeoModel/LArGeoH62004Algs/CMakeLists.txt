################################################################################
# Package: LArGeoH62004Algs
################################################################################

# Declare the package name:
atlas_subdir( LArGeoH62004Algs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/AthContainers
                          Database/RDBAccessSvc
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4RunControl
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArGeoModel/LArGeoEndcap
                          LArCalorimeter/LArGeoModel/LArGeoH6Cryostats
                          LArCalorimeter/LArGeoModel/LArGeoTBEC )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_component( LArGeoH62004Algs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry CaloDetDescrLib AthContainers GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoH6Cryostats LArGeoTBECLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

