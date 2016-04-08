################################################################################
# Package: MuonCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCnv/MuonPrdSelector
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          LArCalorimeter/LArRecEvent
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibNtuple
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          MuonSpectrometer/MuonSimData
                          Simulation/G4Sim/TrackRecord
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkEventPrimitives
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( MuonCalibAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibAlgs
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibEventBase StoreGateLib SGtests MuonIdHelpersLib CscClusterizationLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventInfo xAODEventInfo GeneratorObjects LArRecEvent MuonCalibEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonDigitContainer MuonRDO MuonPrepRawData MuonTrigCoinData MuonSimData TileEvent TileIdentifier TrkEventPrimitives TrigT1Result )

atlas_add_component( MuonCalibAlgs
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuonCalibEventBase StoreGateLib SGtests GeoPrimitives EventInfo xAODEventInfo GeneratorObjects LArRecEvent MuonCalibEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO CscClusterizationLib MuonPrepRawData MuonTrigCoinData MuonSimData TileEvent TileIdentifier TrkEventPrimitives TrigT1Result MuonCalibAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

