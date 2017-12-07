################################################################################
# Package: TrigEgammaAnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaAnalysisTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTrigCalo
   Event/xAOD/xAODTrigEgamma
   Event/xAOD/xAODTrigRinger
   Event/xAOD/xAODTrigger
   Event/xAOD/xAODTruth
   LumiBlock/LumiBlockComps
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
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
   Control/AthenaMonitoring
   Control/StoreGate
   GaudiKernel
   Trigger/TrigConfiguration/TrigConfxAOD )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist Tree )

# Component(s) in the package:
atlas_add_library( TrigEgammaAnalysisToolsLib
   TrigEgammaAnalysisTools/*.h Root/*.cxx
   PUBLIC_HEADERS TrigEgammaAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODEventInfo xAODJet xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger
   xAODTrigger xAODTruth LumiBlockCompsLib EgammaAnalysisInterfacesLib
   RecoToolInterfaces egammaMVACalibLib TrigDecisionToolLib
   TrigEgammaMatchingToolLib TrigEgammaEmulationToolLib TrigConfHLTData
   TrigSteeringEvent TrigHLTMonitoringLib PATCoreLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfxAODLib )

atlas_add_component( TrigEgammaAnalysisTools
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel AsgTools TrigHLTMonitoringLib
   AthenaMonitoringLib TrigDecisionToolLib TrigEgammaMatchingToolLib xAODEgamma
   TrigEgammaAnalysisToolsLib )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_scripts( share/trigEgammaDQ.py share/get_trigEgammaDQ.sh )
