################################################################################
# Package: LArGeoAlgsNV
################################################################################

# Declare the package name:
atlas_subdir( LArGeoAlgsNV )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/IOVSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArGeoBarrel
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArGeoModel/LArGeoEndcap
                          LArCalorimeter/LArGeoModel/LArGeoRAL
                          LArCalorimeter/LArGeoModel/LArGeoTBBarrel
                          LArCalorimeter/LArGeoModel/LArHV )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( LArGeoAlgsNV
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} IOVSvcLib GeoModelKernel GeoModelUtilities LArReadoutGeometry SGTools StoreGateLib SGtests DetDescrConditions GaudiKernel LArGeoBarrel LArGeoCode LArGeoEndcap LArGeoRAL LArGeoTBBarrel LArHV )

# Install files from the package:
atlas_install_headers( LArGeoAlgsNV )

