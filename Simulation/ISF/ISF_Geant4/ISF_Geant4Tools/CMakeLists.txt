################################################################################
# Package: ISF_Geant4Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4Tools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Atlas/G4AtlasAlg
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/SimHelpers
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Geant4/ISF_Geant4Event )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

atlas_add_library( ISF_Geant4ToolsLib
                   ISF_Geant4Tools/*.h
                   INTERFACE
                   PUBLIC_HEADERS ISF_Geant4Tools
                   LINK_LIBRARIES GaudiKernel G4AtlasAlgLib )

# Component(s) in the package:
atlas_add_component( ISF_Geant4Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GaudiKernel G4AtlasInterfaces AthenaBaseComps StoreGateLib SGtests AtlasDetDescr GeneratorObjects G4AtlasToolsLib G4AtlasAlgLib MCTruth SimHelpers ISF_Event ISF_InterfacesLib ISF_Geant4Event ISF_Geant4ToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

