################################################################################
# Package: TRT_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( TRT_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetG4/TRT_G4Utilities
                          InnerDetector/InDetSimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          AtlasTest/TestTools
                           )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
#find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_component( TRT_G4_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel StoreGateLib SGtests GaudiKernel TRT_G4Utilities InDetSimEvent G4AtlasToolsLib MCTruth )

atlas_add_library( TRT_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS TRT_G4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel CxxUtils StoreGateLib SGtests GaudiKernel TRT_G4Utilities InDetSimEvent G4AtlasToolsLib MCTruth )


atlas_add_test( TRT_G4_SDToolConfig_test
                SCRIPT test/TRT_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( TRTSensitiveDetector_gtest
                SOURCES
                test/TRTSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools TRT_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel CxxUtils StoreGateLib SGtests GaudiKernel TRT_G4Utilities InDetSimEvent G4AtlasToolsLib MCTruth
              )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
