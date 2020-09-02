################################################################################
# Package: PixelG4_SD
################################################################################

# Declare the package name:
atlas_subdir( PixelG4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( PixelG4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS PixelG4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth )

atlas_add_component( PixelG4_SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel InDetSimEvent G4AtlasToolsLib MCTruth PixelG4_SDLib )

atlas_add_test( PixelG4_SDToolConfig_test
                SCRIPT test/PixelG4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( PixelSensorSD_gtest
                SOURCES
                test/PixelSensorSD_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools G4AtlasToolsLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} PixelG4_SDLib
)  

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
