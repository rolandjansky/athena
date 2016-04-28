################################################################################
# Package: HGTDG4SD
################################################################################

# Declare the package name:
atlas_subdir( HGTDG4SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
			  Calorimeter/CaloIdentifier
			  Control/CxxUtils
			  Simulation/G4Atlas/G4AtlasTools
			  LArCalorimeter/LArSimEvent
			  Simulation/G4Sim/MCTruth
			  Control/StoreGate )

# External dependencies:
find_package( AtlasCLHEP GaudiInterface Geant4 )

# Component(s) in the package:
atlas_add_library( HGTDG4SD
                   src/*.cxx
                   PUBLIC_HEADERS AtlasDetDescr
                   INCLUDE_DIRS ${ATLASCLHEP_INCLUDE_DIRS} ${GAUDIINTERFACE_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ATLASCLHEP_LIBRARIES} ${GAUDIINTERFACE_LIBRARIES} ${GEANT4_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES CaloIdentifier CxxUtils G4AtlasTools LArSimEvent MCTruth StoreGate)

