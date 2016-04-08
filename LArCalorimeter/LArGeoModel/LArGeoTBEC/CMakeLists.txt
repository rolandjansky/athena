################################################################################
# Package: LArGeoTBEC
################################################################################

# Declare the package name:
atlas_subdir( LArGeoTBEC )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoSpecialShapes
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4RunControl
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArGeoModel/LArGeoEndcap
                          LArCalorimeter/LArGeoModel/LArGeoRAL )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( LArGeoTBECLib
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBEC
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeoModelKernel GeoModelUtilities LArReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} GeoSpecialShapes GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoRAL )

atlas_add_component( LArGeoTBEC
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelKernel GeoModelUtilities LArReadoutGeometry StoreGateLib SGtests GeoSpecialShapes GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoRAL LArGeoTBECLib )

