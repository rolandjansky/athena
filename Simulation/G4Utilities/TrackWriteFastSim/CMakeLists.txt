################################################################################
# Package: TrackWriteFastSim
################################################################################

# Declare the package name:
atlas_subdir( TrackWriteFastSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          Simulation/G4Sim/TrackRecord
                          PRIVATE
                          Control/CxxUtils
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TrackWriteFastSimLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackWriteFastSim
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} StoreGateLib SGtests G4AtlasToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CxxUtils GaudiKernel G4AtlasInterfaces MCTruth )

atlas_add_component( TrackWriteFastSim
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests CxxUtils GaudiKernel G4AtlasInterfaces G4AtlasToolsLib MCTruth TrackWriteFastSimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

