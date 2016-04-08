################################################################################
# Package: G4UserActions
################################################################################

# Declare the package name:
atlas_subdir( G4UserActions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/TrackRecord
                          PRIVATE
                          Control/CxxUtils
                          Event/EventInfo
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Utilities/TrackWriteFastSim )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4UserActions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel G4AtlasToolsLib CxxUtils EventInfo InDetSimEvent MuonSimEvent MCTruth SimHelpers TrackWriteFastSimLib )

# Install files from the package:
atlas_install_headers( G4UserActions )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

