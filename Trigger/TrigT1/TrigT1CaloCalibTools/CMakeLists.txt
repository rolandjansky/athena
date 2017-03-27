################################################################################
# Package: TrigT1CaloCalibTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCondSvc
                          Trigger/TrigT1/TrigT1CaloMonitoringTools
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1CaloUtils
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaMonitoring
                          Event/EventInfo
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArCabling
                          Tools/LWHists
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibToolsLib
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier xAODTrigL1Calo GaudiKernel LArIdentifier LArRecConditions TileEvent TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloToolInterfaces CaloDetDescrLib StoreGateLib SGtests LArRecUtilsLib TileConditionsLib TrigT1CaloMonitoringToolsLib TrigT1CaloUtilsLib CaloTriggerToolLib AthenaMonitoringLib LArCablingLib TrigT1CaloEventLib  TrigT1CaloCondSvcLib
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloIdentifier EventInfo LArRawEvent LWHists TrigT1Interfaces )

atlas_add_component( TrigT1CaloCalibTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloDetDescrLib AsgTools AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier xAODTrigL1Calo GaudiKernel LArIdentifier LArRecConditions LArRecUtilsLib TileConditionsLib TileEvent TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib CaloEvent CaloIdentifier CaloTriggerToolLib AthenaMonitoringLib EventInfo LArRawEvent LArCablingLib LWHists TrigT1CaloEventLib TrigT1Interfaces TrigT1CaloCalibToolsLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )

