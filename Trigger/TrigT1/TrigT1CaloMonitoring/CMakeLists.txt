################################################################################
# Package: TrigT1CaloMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaMonitoring
                          Control/AthContainers
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTrigL1Calo
			  Event/xAOD/xAODEgamma
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/Jet/JetInterface
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          Tools/LWHists
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfL1Data
			  Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCalibTools
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloMonitoringTools
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCondSvc
                          Trigger/TrigT1/TrigT1CaloUtils
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT1CaloMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier AthenaMonitoringLib AthContainers SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier EventInfo xAODJet xAODTrigL1Calo xAODEgamma GaudiKernel AnalysisTriggerEvent JetInterface TileConditionsLib TileEvent LWHists VxVertex TrigDecisionToolLib TrigConfL1Data TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCalibToolsLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_headers( TrigT1CaloMonitoring )
atlas_install_joboptions( share/*.py )

