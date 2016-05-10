################################################################################
# Package: TrigJetMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigJetMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/StoreGate
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigJetMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigJetMonitoring
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODEventInfo xAODJet xAODTrigger GaudiKernel AnalysisTriggerEvent TrigCaloEvent TrigHLTMonitoringLib StoreGateLib SGtests TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TrigSteeringEvent )

atlas_add_component( TrigJetMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} xAODEventInfo xAODJet xAODTrigger GaudiKernel AnalysisTriggerEvent TrigCaloEvent TrigHLTMonitoringLib StoreGateLib SGtests TrigDecisionToolLib TrigSteeringEvent TrigJetMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

