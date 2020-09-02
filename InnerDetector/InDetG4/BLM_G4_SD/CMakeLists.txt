################################################################################
# Package: BLM_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( BLM_G4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( BLM_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS BLM_G4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth
                 )

atlas_add_component( BLM_G4_SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth BLM_G4_SDLib )

atlas_add_test( BLM_G4_SDToolConfig_test
                SCRIPT test/BLM_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( BLMSensorSD_gtest
                SOURCES
                test/BLMSensorSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools BLM_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth
              )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
