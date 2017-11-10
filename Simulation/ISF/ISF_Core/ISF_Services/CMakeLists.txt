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
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/Barcode/BarcodeEvent
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Sim/TrackRecord
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Interfaces
                          TileCalorimeter/TileSimEvent
                          Tools/PmbCxxUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( GTest )
find_package( GMock )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_Services
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} GaudiKernel CaloIdentifier CaloSimEvent AthenaBaseComps StoreGateLib SGtests AtlasDetDescr GeneratorObjects InDetSimEvent MuonSimEvent G4AtlasInterfaces SimHelpers ISF_Event ISF_Interfaces TileSimEvent PmbCxxUtils )

# Tests in the package:
atlas_add_test( InputConverter_test
                SOURCES
                test/InputConverter_test.cxx
                src/InputConverter.cxx
                INCLUDE_DIRS
                ${HEPPDT_INCLUDE_DIRS}
                ${GTEST_INCLUDE_DIRS}
                ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES
                ISF_Event
                ISF_Interfaces
                ${HEPPDT_LIBRARIES}
                AthenaBaseComps
                GeneratorObjects
                ${GTEST_LIBRARIES}
                ${GMOCK_LIBRARIES}
                )

# Install files from the package:
atlas_install_headers( ISF_Services )
atlas_install_python_modules( python/*.py )

