################################################################################
# Package: RpcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( RpcRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonRPC_Cabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonGeoModel
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils
                          Reconstruction/MuonIdentification/muonEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonTGRecTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkMultiComponentStateOnSurface
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Result
                          PhysicsAnalysis/MuonID/MuonSelectorTools )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( RpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests xAODEventInfo xAODMuon xAODTrigger xAODTracking GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonGeoModelLib MuonRDO MuonRIO_OnTrack MuonRPC_CablingLib MuonTrigCoinData MuonDQAUtilsLib muonEvent TrigDecisionToolLib TrigT1Interfaces GeoPrimitives EventPrimitives MuonDigitContainer MuonIdHelpersLib MuonPrepRawData TrkEventPrimitives TrkMeasurementBase TrkMultiComponentStateOnSurface TrkTrack TrigConfL1Data TrigT1Result )

# Install files from the package:
atlas_install_headers( RpcRawDataMonitoring )
atlas_install_joboptions( share/*.py )

atlas_install_python_modules( python/*.py )
