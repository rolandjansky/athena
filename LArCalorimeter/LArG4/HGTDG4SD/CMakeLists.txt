################################################################################
# Package: HGTDG4SD
################################################################################

# Declare the package name:
atlas_subdir( HGTDG4SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
        Calorimeter/CaloIdentifier
        Control/CxxUtils
        Simulation/G4Atlas/G4AtlasTools
        LArCalorimeter/LArSimEvent
        Simulation/G4Sim/MCTruth
        Control/StoreGate
        GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( HGTDG4SD
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} StoreGateLib
   CaloIdentifier CxxUtils G4AtlasToolsLib LArSimEvent MCTruth GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
