################################################################################
# Package: TrigTauMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigTauMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          LumiBlock/LumiBlockComps
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigTauAnalysis/TrigTauEmulation
                         # Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigDecisionInterface
                          Trigger/TrigMonitoring/TrigHLTMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TrigTauMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigCaloEvent TrigInDetEvent TrigParticle TrigSteeringEvent AthenaMonitoringLib StoreGateLib SGtests EventInfo xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTau xAODTracking xAODTrigger xAODTruth GaudiKernel AnalysisUtilsLib tauEvent VxVertex TrigDecisionToolLib  TrigConfL1Data TrigHLTMonitoringLib )

# Install files from the package:
atlas_install_headers( TrigTauMonitoring )
atlas_install_python_modules( python/*.py )

