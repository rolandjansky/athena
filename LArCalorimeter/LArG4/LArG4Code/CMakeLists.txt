################################################################################
# Package: LArG4Code
################################################################################

# Declare the package name:
atlas_subdir( LArG4Code )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

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

atlas_add_test( LArG4CalibSD_gtest
                SOURCES test/LArG4CalibSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                DEFINITIONS ${CLHEP_DEFINITIONS}
                LINK_LIBRARIES TestTools LArG4Code ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests GaudiKernel CaloIdentifier AthenaKernel CxxUtils MCTruth )

atlas_add_test( LArG4SimpleSD_gtest
                SOURCES test/LArG4SimpleSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                DEFINITIONS ${CLHEP_DEFINITIONS}
                LINK_LIBRARIES TestTools LArG4Code ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests GaudiKernel CaloIdentifier AthenaKernel CxxUtils MCTruth )

atlas_add_test( SDWrapper_gtest
                SOURCES test/SDWrapper_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                DEFINITIONS ${CLHEP_DEFINITIONS}
                LINK_LIBRARIES TestTools LArG4Code ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr CaloSimEvent LArGeoCode LArSimEvent CaloG4SimLib G4AtlasToolsLib StoreGateLib SGtests GaudiKernel CaloIdentifier AthenaKernel CxxUtils MCTruth )

atlas_install_joboptions( share/optionForTest.txt )

