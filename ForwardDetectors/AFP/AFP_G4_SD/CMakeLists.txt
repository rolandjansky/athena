################################################################################
# Package: AFP_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( AFP_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          ForwardDetectors/AFP/AFP_Geometry
                          ForwardDetectors/AFP/AFP_SimEv
                          Simulation/G4Atlas/G4AtlasTools
                          AtlasTest/TestTools
                           )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( AFP_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS AFP_G4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests AFP_Geometry AFP_SimEv G4AtlasToolsLib
                 )

atlas_add_component( AFP_G4_SD
                     src/components/*.cxx
                     LINK_LIBRARIES AFP_G4_SDLib )

atlas_add_test( AFP_SensitiveDetector_gtest
                SOURCES
                test/AFP_SensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools AFP_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests AFP_Geometry AFP_SimEv G4AtlasToolsLib
              )

atlas_add_test( AFP_SiDSensitiveDetector_gtest
                SOURCES
                test/AFP_SiDSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools AFP_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests AFP_Geometry AFP_SimEv G4AtlasToolsLib
              )

atlas_add_test( AFP_TDSensitiveDetector_gtest
                SOURCES
                test/AFP_TDSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools AFP_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests AFP_Geometry AFP_SimEv G4AtlasToolsLib
              )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
