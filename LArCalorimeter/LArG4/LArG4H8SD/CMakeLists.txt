################################################################################
# Package: LArG4H8SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4H8SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4Code )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4H8SD
                   src/*.cc
                   src/components/*.cxx
                   PUBLIC_HEADERS LArG4H8SD
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloG4SimLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel CaloSimEvent CxxUtils GaudiKernel LArG4Code )

