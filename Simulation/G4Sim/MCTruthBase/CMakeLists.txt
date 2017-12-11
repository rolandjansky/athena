################################################################################
# Package: MCTruthBase
################################################################################

# Declare the package name:
atlas_subdir( MCTruthBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          Simulation/G4Sim/SimHelpers
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/TrackRecord
                          DetectorDescription/AtlasDetDescr
                          Simulation/ISF/ISF_Geant4/ISF_Geant4Event )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepMC )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( MCTruthBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS MCTruthBase
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel StoreGateLib SGtests G4AtlasToolsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils
                   G4AtlasInterfaces MCTruth SimHelpers ISF_Interfaces
                   AtlasDetDescr ISF_Geant4Event
                 )

atlas_add_component( MCTruthBase
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES}
                     ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES}
                     ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel
                     AthenaBaseComps CxxUtils StoreGateLib SGtests
                     G4AtlasInterfaces G4AtlasToolsLib MCTruth
                     SimHelpers ISF_Interfaces
                     AtlasDetDescr MCTruthBaseLib
                     ISF_Geant4Event )

# Install files from the package:
atlas_install_python_modules( python/*.py )
