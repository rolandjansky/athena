################################################################################
# Package: LArG4Code
################################################################################

# Declare the package name:
atlas_subdir( LArG4Code )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          DetectorDescription/AtlasDetDescr
                          LArCalorimeter/LArGeoModel/LArGeoCode
                          LArCalorimeter/LArSimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/CxxUtils
                          Simulation/G4Sim/MCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

add_definitions(-DLARG4NOROOT)

# Component(s) in the package:
atlas_add_library( LArG4Code
                   src/*.cc
                   src/*.cxx
                   PUBLIC_HEADERS LArG4Code
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests GaudiKernel
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaKernel CxxUtils MCTruth )

atlas_add_dictionary( LArG4CodeEnums
                      LArG4Code/LArG4EnumDefs.h
                      LArG4Code/selectionEnums.xml
                      INCLUDE_DIRS  ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests GaudiKernel LArG4Code )
