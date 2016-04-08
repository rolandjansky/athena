################################################################################
# Package: LArG4Code
################################################################################

# Declare the package name:
atlas_subdir( LArG4Code )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArSimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/StoreGate
                          Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

add_definitions(-DLARG4NOROOT)

# Component(s) in the package:
atlas_add_library( LArG4Code
                   src/*.cc
                   PUBLIC_HEADERS LArG4Code
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaKernel MCTruth )

