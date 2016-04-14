################################################################################
# Package: MCTruthBase
################################################################################

# Declare the package name:
atlas_subdir( MCTruthBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/SimHelpers
                          PRIVATE
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Utilities/TrackWriteFastSim )

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
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel SimHelpers G4AtlasToolsLib TrackWriteFastSimLib
                   PRIVATE_LINK_LIBRARIES G4AtlasInterfaces MCTruth )

atlas_add_component( MCTruthBase
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel G4AtlasToolsLib SimHelpers G4AtlasInterfaces MCTruth TrackWriteFastSimLib MCTruthBaseLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

