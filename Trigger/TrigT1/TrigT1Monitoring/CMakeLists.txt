################################################################################
# Package: TrigT1Monitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigT1Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaMonitoring
                          Control/DataModel
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          LArCalorimeter/LArRecUtils
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tools/AtlasDoxygen
                          Tools/LWHists
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/L1Topo/L1TopoRDO
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCondSvc
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloMonitoringTools
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( TrigT1Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloIdentifier CaloTriggerToolLib AthenaMonitoringLib DataModel Identifier xAODTrigL1Calo GaudiKernel LArRecUtilsLib TileCalibBlobObjs TileConditionsLib TileEvent TileIdentifier LWHists TrigConfL1Data L1TopoRDO TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1Interfaces TrigT1Result TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_headers( TrigT1Monitoring )
atlas_install_joboptions( share/*.py )

