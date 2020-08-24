################################################################################
# Package: BCM_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( BCM_G4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( BCM_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS BCM_G4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils AthenaKernel StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth                 )

atlas_add_component( BCM_G4_SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth BCM_G4_SDLib )

atlas_add_test( BCM_G4_SDToolConfig_test
                SCRIPT test/BCM_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( BCMSensorSD_gtest
                SOURCES
                test/BCMSensorSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools BCM_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils AthenaKernel StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth
              )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
