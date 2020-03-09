################################################################################
# Package: LArGeoAlgsNV
################################################################################

# Declare the package name:
atlas_subdir( LArGeoAlgsNV )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
			              DetectorDescription/GeoPrimitives
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
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArGeoAlgsNV
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} ${EIGEN_LIBRARIES} GeoModelUtilities LArReadoutGeometry SGTools StoreGateLib SGtests DetDescrConditions GaudiKernel LArGeoBarrel LArGeoCode LArGeoEndcap LArGeoRAL LArGeoTBBarrel LArHV )

# Install files from the package:
atlas_install_python_modules( python/*.py )

if( NOT SIMULATIONBASE )
  atlas_add_test( LArGMConfigTest SCRIPT python -m LArGeoAlgsNV.LArGMConfig POST_EXEC_SCRIPT nopost.sh )
endif()
