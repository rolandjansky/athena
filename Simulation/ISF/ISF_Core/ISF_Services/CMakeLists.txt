################################################################################
# Package: ISF_Services
################################################################################

# Declare the package name:
atlas_subdir( ISF_Services )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Generators/GeneratorObjects
                          Generators/TruthUtils
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/Barcode/BarcodeEvent
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Sim/TrackRecord
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Interfaces
                          TileCalorimeter/TileSimEvent
                          Tools/PmbCxxUtils )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component( ISF_Services
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} ${HEPMC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} GaudiKernel CaloIdentifier CaloSimEvent AthenaBaseComps StoreGateLib SGtests AtlasDetDescr GeneratorObjects InDetSimEvent MuonSimEvent G4AtlasInterfaces MCTruth SimHelpers ISF_Event ISF_Interfaces TileSimEvent PmbCxxUtils TruthUtils )

atlas_add_test( InputConverter_test
                SOURCES
                  test/InputConverter_test.cxx src/InputConverter.cxx
                INCLUDE_DIRS
                  ${GTEST_INCLUDE_DIRS}
                  ${GMOCK_INCLUDE_DIRS}
                  ${ROOT_INCLUDE_DIRS}
                  ${GEANT4_INCLUDE_DIRS}
                  ${HEPMC_INCLUDE_DIRS}
                  ${CLHEP_INCLUDE_DIRS}
                  ${HEPPDT_INCLUDE_DIRS}
                LINK_LIBRARIES
                  ${GTEST_LIBRARIES}
                  ${GMOCK_LIBRARIES}
                  ${ROOT_LIBRARIES}
                  ${GEANT4_LIBRARIES}
                  ${HEPMC_LIBRARIES}
                  ${CLHEP_LIBRARIES}
                  ${HEPPDT_LIBRARIES}
                  GaudiKernel
                  CaloIdentifier
                  CaloSimEvent
                  AthenaBaseComps
                  StoreGateLib
                  InDetSimEvent
                  MuonSimEvent
                  ISF_Event
                  ISF_Interfaces
                  PmbCxxUtils
                  MCTruth
                ENVIRONMENT
                  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                )

atlas_add_test( TruthSvc_test
                SOURCES
                  test/TruthSvc_test.cxx src/TruthSvc.cxx
                INCLUDE_DIRS
                  ${GTEST_INCLUDE_DIRS}
                  ${GMOCK_INCLUDE_DIRS}
                  ${ROOT_INCLUDE_DIRS}
                  ${GEANT4_INCLUDE_DIRS}
                  ${HEPMC_INCLUDE_DIRS}
                  ${CLHEP_INCLUDE_DIRS}
                  ${HEPPDT_INCLUDE_DIRS}
                LINK_LIBRARIES
                  ${GTEST_LIBRARIES}
                  ${GMOCK_LIBRARIES}
                  ${ROOT_LIBRARIES}
                  ${GEANT4_LIBRARIES}
                  ${HEPMC_LIBRARIES}
                  ${CLHEP_LIBRARIES}
                  ${HEPPDT_LIBRARIES}
                  GaudiKernel
                  CaloIdentifier
                  CaloSimEvent
                  AthenaBaseComps
                  StoreGateLib
                  InDetSimEvent
                  MuonSimEvent
                  ISF_Event
                  ISF_Interfaces
                  PmbCxxUtils
                  MCTruth
                  TruthUtils
                ENVIRONMENT
                  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                )

# Install files from the package:
atlas_install_python_modules( python/*.py )

