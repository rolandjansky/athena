################################################################################
# Package: TileFastCaloSim
################################################################################

# Declare the package name:
atlas_subdir( TileFastCaloSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventContainers
                          GaudiKernel
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimEvent
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileG4/TileG4Interfaces
                          TileCalorimeter/TileGeoModel
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TileFastCaloSim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel GaudiKernel ISF_FastCaloSimEvent TileConditionsLib TileDetDescr TileGeoModelLib TileG4InterfacesLib TileSimEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )

