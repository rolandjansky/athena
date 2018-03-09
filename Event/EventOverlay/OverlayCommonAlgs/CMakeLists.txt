################################################################################
# Package: OverlayCommonAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayCommonAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCnvInterfaces
                          Event/xAOD/xAODJet
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          Database/PersistentDataModel
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvc
                          Event/EventInfo
                          Event/EventOverlay/OverlayAlgBase
                          Generators/GeneratorObjects
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Reconstruction/RecEvent
                          Simulation/G4Sim/TrackRecord
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSimEvent
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( OverlayCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel CaloSimEvent AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests PersistentDataModel Identifier ByteStreamCnvSvcLib EventInfo OverlayAlgBase GeneratorObjects InDetBCM_RawData InDetRawData InDetSimData InDetSimEvent LArRawEvent LArSimEvent MuonDigitContainer MuonRDO MuonSimData MuonSimEvent RecEvent TileEvent TileSimEvent TrigConfHLTData TrigSteeringEvent TrigT1CaloEventLib TrigT1Interfaces TrigT1Result xAODJet )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/OverlayFlags.py )
