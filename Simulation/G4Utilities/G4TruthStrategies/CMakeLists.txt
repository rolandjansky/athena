################################################################################
# Package: G4TruthStrategies
################################################################################

# Declare the package name:
atlas_subdir( G4TruthStrategies )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Simulation/G4Sim/MCTruthBase
                          PRIVATE
                          Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4TruthStrategies
                   src/*.cxx
                   PUBLIC_HEADERS G4TruthStrategies
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES MCTruthBaseLib
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} MCTruth )

