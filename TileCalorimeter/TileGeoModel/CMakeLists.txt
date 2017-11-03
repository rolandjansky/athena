################################################################################
# Package: TileGeoModel
################################################################################

# Declare the package name:
atlas_subdir( TileGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          TileCalorimeter/TileDetDescr
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/RDBAccessSvc
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( TileGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS TileGeoModel
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeoModelKernel GeoModelUtilities TileDetDescr StoreGateLib SGtests CaloDetDescrLib TileConditionsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier SGTools GaudiKernel TileIdentifier )

atlas_add_component( TileGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GeoModelKernel GeoModelUtilities TileDetDescr CaloDetDescrLib CaloIdentifier SGTools GaudiKernel TileConditionsLib TileIdentifier TileGeoModelLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )

