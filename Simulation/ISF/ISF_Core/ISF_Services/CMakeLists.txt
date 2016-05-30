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
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/Barcode/BarcodeServices
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Sim/TrackRecord
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          TileCalorimeter/TileSimEvent
                          Tools/PmbCxxUtils )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_Services
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel CaloIdentifier CaloSimEvent AthenaBaseComps StoreGateLib SGtests AtlasDetDescr GeneratorObjects InDetSimEvent MuonSimEvent BarcodeServicesLib G4AtlasInterfaces SimHelpers ISF_Event ISF_Interfaces TileSimEvent PmbCxxUtils )

# Install files from the package:
atlas_install_headers( ISF_Services )
atlas_install_python_modules( python/*.py )

