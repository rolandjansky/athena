################################################################################
# Package: TileGeoModel
################################################################################

# Declare the package name:
atlas_subdir( TileGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          TileCalorimeter/TileDetDescr
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/SGTools
                          Database/RDBAccessSvc
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TileGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS TileGeoModel
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities TileDetDescr StoreGateLib SGtests CaloDetDescrLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier SGTools GaudiKernel TileIdentifier )

atlas_add_component( TileGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES TileGeoModelLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )


if( NOT SIMULATIONBASE )
  atlas_add_test( TileGMConfig    SCRIPT python -m TileGeoModel.TileGMConfig POST_EXEC_SCRIPT nopost.sh )
endif()

