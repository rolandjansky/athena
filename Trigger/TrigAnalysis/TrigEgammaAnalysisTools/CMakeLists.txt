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
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODCaloRings
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaMVACalib
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigEgammaMatchingTool
                          Trigger/TrigAnalysis/TrigEgammaEmulationTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfxAOD )
# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist Tree )

# Component(s) in the package:
atlas_add_library( TrigEgammaAnalysisToolsLib
   Root/*.cxx
   PUBLIC_HEADERS TrigEgammaAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODEventInfo xAODJet xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODMissingET
   xAODTrigger xAODCaloRings xAODTruth LumiBlockCompsLib EgammaAnalysisInterfacesLib
   RecoToolInterfaces egammaMVACalibAnalysisLib TrigDecisionToolLib
   TrigEgammaMatchingToolLib TrigEgammaEmulationToolLib TrigConfHLTData
   TrigSteeringEvent TrigHLTMonitoringLib PATCoreLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfxAODLib )

atlas_add_component( TrigEgammaAnalysisTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
                     TrigEgammaAnalysisToolsLib
                     AthenaBaseComps AthenaMonitoringLib AthenaMonitoringKernelLib
                     StoreGateLib SGtests GaudiKernel  )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_generic( share/trigEgammaDQ.py share/get_trigEgammaDQ.sh
                        DESTINATION share
                        EXECUTABLE )

