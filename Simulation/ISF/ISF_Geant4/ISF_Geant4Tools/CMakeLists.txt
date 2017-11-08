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
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Generators/GeneratorObjects
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/SimHelpers
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Geant4/ISF_Geant4Event
                          Simulation/ISF/ISF_Geant4/ISF_Geant4Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Event )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_Geant4Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GaudiKernel G4AtlasInterfaces AthenaBaseComps CxxUtils StoreGateLib SGtests AtlasDetDescr GeneratorObjects G4AtlasToolsLib MCTruth SimHelpers ISF_Event ISF_Interfaces ISF_Geant4Event )

# Install files from the package:
atlas_install_headers( ISF_Geant4Tools )
atlas_install_python_modules( python/*.py )

