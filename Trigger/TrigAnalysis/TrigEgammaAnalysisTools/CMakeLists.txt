################################################################################
# Package: TrigEgammaAnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaAnalysisTools )

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
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaMVACalib
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigEgammaMatchingTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfxAOD )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigEgammaAnalysisTools
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODEventInfo xAODJet xAODPrimitives xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger xAODTruth LumiBlockCompsLib ElectronPhotonSelectorToolsLib RecoToolInterfaces egammaMVACalibLib TrigDecisionToolLib TrigEgammaMatchingToolLib TrigConfHLTData TrigSteeringEvent TrigHLTMonitoringLib AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigEgammaAnalysisTools )
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_generic( macros/trigEgammaDQ.py macros/get_trigEgammaDQ.sh 
                        DESTINATION share/bin
                        EXECUTABLE )

