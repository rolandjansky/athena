################################################################################
# Package: LArG4H8SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4H8SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          Control/CxxUtils
                          Control/StoreGate
                          LArCalorimeter/LArG4/LArG4Barrel
                          LArCalorimeter/LArG4/LArG4Code )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( LArG4H8SD
                   src/*.cc
                   PUBLIC_HEADERS LArG4H8SD
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloG4SimLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloSimEvent CxxUtils LArG4Barrel LArG4Code )

