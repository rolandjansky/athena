################################################################################
# Package: TrigT1CaloCalibUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCalibUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCalibTools
                          Trigger/TrigT1/TrigT1CaloCondSvc
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloMonitoringTools
                          Trigger/TrigT1/TrigT1CaloUtils
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArRecUtils
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier EventInfo xAODTrigL1Calo GaudiKernel TrigConfHLTData TrigConfL1Data TrigSteeringEvent TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces CaloTriggerToolLib TrigT1CaloCalibToolsLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloUtilsLib CaloDetDescrLib StoreGateLib SGtests LArRecUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CxxUtils xAODEventInfo AtlasCLHEP_RandomGenerators TrigT1CaloToolInterfaces TrigT1Interfaces )

atlas_add_component( TrigT1CaloCalibUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier CaloTriggerToolLib AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier EventInfo xAODTrigL1Calo GaudiKernel TrigConfHLTData TrigConfL1Data TrigSteeringEvent TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCalibToolsLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloUtilsLib CaloDetDescrLib CaloEvent CxxUtils StoreGateLib SGtests xAODEventInfo LArRecUtilsLib AtlasCLHEP_RandomGenerators TrigT1CaloToolInterfaces TrigT1Interfaces TrigT1CaloCalibUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

