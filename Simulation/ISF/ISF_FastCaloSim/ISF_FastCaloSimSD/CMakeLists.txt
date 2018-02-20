################################################################################
# Package: ISF_FastCaloSimSD
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimSD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          TileCalorimeter/TileG4/TileG4Interfaces
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/CxxUtils
                          Control/AthenaBaseComps
                          Generators/GeneratorObjects
                          LArCalorimeter/LArG4/LArG4Code
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimEvent
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_FastCaloSimSD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel G4AtlasInterfaces G4AtlasToolsLib CaloDetDescrLib CaloIdentifier CxxUtils GeneratorObjects LArG4Code ISF_FastCaloSimEvent Identifier )

# Install files from the package:
atlas_install_headers( ISF_FastCaloSimSD )
atlas_install_python_modules( python/*.py )

