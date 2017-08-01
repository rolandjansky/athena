################################################################################
# Package: TileGeoG4Calib
################################################################################

# Declare the package name:
atlas_subdir( TileGeoG4Calib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/AtlasDoxygen
                          PRIVATE
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/CxxUtils
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileG4/TileGeoG4SD
                          TileCalorimeter/TileSimEvent
                          TileCalorimeter/TileG4/TileG4Interfaces
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TileGeoG4Calib
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} CaloG4SimLib CaloIdentifier CaloSimEvent CxxUtils StoreGateLib SGtests GeoModelUtilities GaudiKernel G4AtlasToolsLib MCTruth TileDetDescr TileGeoG4SDLib TileSimEvent PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )

