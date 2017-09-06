################################################################################
# Package: TgcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TgcRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils
                          PRIVATE
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigger
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonRDO
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Graf Hist Core Tree MathCore RIO pthread Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TgcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonTrigCoinData MuonDQAUtilsLib Identifier xAODEventInfo xAODMuon xAODTrigger MuonDigitContainer MuonRDO AnalysisTriggerEvent muonEvent TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TgcRawDataMonitoring )
atlas_install_joboptions( share/*.py )

