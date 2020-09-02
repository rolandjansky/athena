################################################################################
# Package: LUCID_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( LUCID_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          ForwardDetectors/LUCID/LUCID_GeoModel
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/HitManagement
                          AtlasTest/TestTools
                         )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( LUCID_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS LUCID_G4_SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests LUCID_GeoModelLib LUCID_SimEvent G4AtlasToolsLib HitManagement )

atlas_add_component( LUCID_G4_SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel StoreGateLib SGtests LUCID_GeoModelLib LUCID_SimEvent G4AtlasToolsLib HitManagement LUCID_G4_SDLib )

atlas_add_test( LUCID_SensitiveDetector_gtest
                SOURCES test/LUCID_SensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools pthread LUCID_G4_SDLib ${GTEST_LIBRARIES} ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests LUCID_GeoModelLib LUCID_SimEvent G4AtlasToolsLib HitManagement)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
