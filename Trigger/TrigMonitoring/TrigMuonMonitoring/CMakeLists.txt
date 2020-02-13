################################################################################
# Package: TrigMuonMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/AthenaMonitoring
                          Event/EventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigObjectMatching
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/DecisionHandling )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TrigMuonMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TrigInDetEvent TrigMuonEvent TrigHLTMonitoringLib 
                     AthenaMonitoringLib TrigDecisionToolLib TrigObjectMatchingLib EventInfo xAODMuon xAODPrimitives 
                     xAODTracking xAODTrigMuon xAODTrigger GaudiKernel AnalysisTriggerEvent muonEvent VxVertex 
                     TrigConfHLTData TrigSteeringEvent DecisionHandlingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

