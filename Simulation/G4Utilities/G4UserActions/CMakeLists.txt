################################################################################
# Package: G4UserActions
################################################################################

# Declare the package name:
atlas_subdir( G4UserActions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/TrackRecord
                          PRIVATE
                          Control/CxxUtils
                          Event/EventInfo
                          Generators/TruthUtils
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Utilities/TrackWriteFastSim
                          Simulation/G4Utilities/G4DebuggingTools
                          Tools/PathResolver )


# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4UserActions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel G4AtlasInterfaces G4AtlasToolsLib CxxUtils EventInfo TruthUtils InDetSimEvent MuonSimEvent MCTruth SimHelpers TrackWriteFastSimLib G4DebuggingHelperLib PathResolver)

# Install files from the package:
atlas_install_headers( G4UserActions )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat ) 

