################################################################################
# Package: ISF_Services
################################################################################

# Declare the package name:
atlas_subdir( ISF_Services )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( HepPDT )
find_package( GTest )
find_package( GMock )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component( ISF_Services
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}  ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} AtlasHepMCLib ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} GaudiKernel BarcodeInterfacesLib CaloIdentifier CaloSimEvent AthenaBaseComps StoreGateLib SubDetectorEnvelopesLib SGtests AtlasDetDescr GeneratorObjects InDetSimEvent MuonSimEvent G4AtlasInterfaces MCTruth SimHelpers ISF_Event ISF_HepMC_Interfaces ISF_InterfacesLib TileSimEvent PmbCxxUtils TruthUtils )

# Tests in the package:
atlas_add_test( InputConverter_test
                SOURCES
                test/InputConverter_test.cxx
                src/InputConverter.cxx
                INCLUDE_DIRS
                ${GEANT4_INCLUDE_DIRS}
                ${HEPPDT_INCLUDE_DIRS}
                ${GTEST_INCLUDE_DIRS}
                ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES
                ISF_Event
                ISF_HepMC_Interfaces
                ISF_InterfacesLib
                ${GEANT4_LIBRARIES}
                ${HEPPDT_LIBRARIES}
                AthenaBaseComps
                BarcodeInterfacesLib
                GeneratorObjects
                MCTruth
                ${GTEST_LIBRARIES}
                ${GMOCK_LIBRARIES}
                ENVIRONMENT
                "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                )
# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( ISF_Services_InputConverter_test PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( TruthSvc_test
                SOURCES
                  test/TruthSvc_test.cxx src/TruthSvc.cxx
                INCLUDE_DIRS
                  ${GTEST_INCLUDE_DIRS}
                  ${GMOCK_INCLUDE_DIRS}
                  ${ROOT_INCLUDE_DIRS}
                  ${GEANT4_INCLUDE_DIRS}
                  
                  ${CLHEP_INCLUDE_DIRS}
                  ${HEPPDT_INCLUDE_DIRS}
                LINK_LIBRARIES
                  ${GTEST_LIBRARIES}
                  ${GMOCK_LIBRARIES}
                  ${ROOT_LIBRARIES}
                  ${GEANT4_LIBRARIES}
                  AtlasHepMCLib
                  ${CLHEP_LIBRARIES}
                  ${HEPPDT_LIBRARIES}
                  GaudiKernel
                  BarcodeInterfacesLib
                  CaloIdentifier
                  CaloSimEvent
                  AthenaBaseComps
                  StoreGateLib
                  InDetSimEvent
                  MuonSimEvent
                  ISF_Event
                  ISF_HepMC_Interfaces
                  ISF_InterfacesLib
                  PmbCxxUtils
                  MCTruth
                  TruthUtils
                ENVIRONMENT
                  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                )

#test ISF_ServicesConfigNew
atlas_add_test( ISF_ServicesConfigNew_test
                SCRIPT test/ISF_ServicesConfigNew_test.py
                PROPERTIES TIMEOUT 300 )

# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( ISF_Services_TruthSvc_test PROPERTIES ENABLE_EXPORTS True )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

