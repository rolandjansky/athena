################################################################################
# Package: TrigEgammaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODTrigRinger
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODCaloRings
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaMVACalib
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigEgammaAnalysisTools
                          Trigger/TrigAnalysis/TrigEgammaMatchingTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfxAOD )
# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist Tree )


atlas_add_component( TrigEgammaMonitoring
                     src/*.h
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AsgTools xAODCaloEvent xAODMissingET xAODEgamma xAODEventInfo xAODJet xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODCaloRings xAODTrigger xAODTruth LumiBlockCompsLib ElectronPhotonSelectorToolsLib egammaMVACalibLib TrigDecisionToolLib TrigEgammaMatchingToolLib TrigConfHLTData TrigSteeringEvent TrigHLTMonitoringLib AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests GaudiKernel PATCoreLib TrigEgammaAnalysisToolsLib)

# Install files from the package:



# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )















