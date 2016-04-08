################################################################################
# Package: ISF_Geant4UserActions
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4UserActions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Generators/GeneratorObjects
                          LArCalorimeter/LArG4/LArG4Barrel
                          LArCalorimeter/LArG4/LArG4Code
                          LArCalorimeter/LArG4/LArG4EC
                          LArCalorimeter/LArG4/LArG4FCAL
                          LArCalorimeter/LArG4/LArG4HEC
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimEvent
                          TileCalorimeter/TileG4/TileGeoG4SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_Geant4UserActions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4AtlasToolsLib CaloDetDescrLib CaloIdentifier GeneratorObjects LArG4Barrel LArG4Code LArG4EC LArG4FCAL LArG4HEC ISF_FastCaloSimEvent TileGeoG4SDLib )

# Install files from the package:
atlas_install_headers( ISF_Geant4UserActions )
atlas_install_python_modules( python/*.py )

