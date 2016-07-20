################################################################################
# Package: TrigCostMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigCostMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Database/CoolLumiUtilities
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfJobOptData
                          Trigger/TrigEvent/TrigMonitoringEvent
                          Trigger/TrigSteer/TrigSteering
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Control/AthenaKernel
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfStorage
                          Trigger/TrigDataAccess/TrigDataAccessMonitoring
                          Trigger/TrigDataAccess/TrigSerializeResult
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )
find_package( tdaq-common )
find_package( tdaq-common COMPONENTS CTPfragment )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrigCostMonitorLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCostMonitor
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps xAODEventInfo GaudiKernel TrigConfJobOptData TrigMonitoringEvent AthenaMonitoringLib StoreGateLib SGtests CoolLumiUtilitiesLib TrigDecisionToolLib TrigSteeringLib TrigTimeAlgsLib TrigDataAccessMonitoringLib TrigSerializeResultLib TrigNavigationLib TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel TruthHelper GeneratorObjects PathResolver TrigConfHLTData TrigConfL1Data TrigConfStorage TrigSteeringEvent TrigT1Interfaces TrigT1Result )

atlas_add_component( TrigCostMonitor
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests CoolLumiUtilitiesLib xAODEventInfo GaudiKernel TrigDecisionToolLib TrigConfJobOptData TrigMonitoringEvent TrigSteeringLib TrigTimeAlgsLib AthenaKernel TruthHelper GeneratorObjects PathResolver TrigConfHLTData TrigConfL1Data TrigConfStorage TrigDataAccessMonitoringLib TrigSerializeResultLib TrigNavigationLib TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigT1Result TrigCostMonitorLib )

atlas_add_dictionary( TrigCostMonitorDict
                      TrigCostMonitor/TrigCostMonitorDict.h
                      TrigCostMonitor/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests CoolLumiUtilitiesLib xAODEventInfo GaudiKernel TrigDecisionToolLib TrigConfJobOptData TrigMonitoringEvent TrigSteeringLib TrigTimeAlgsLib AthenaKernel TruthHelper GeneratorObjects PathResolver TrigConfHLTData TrigConfL1Data TrigConfStorage TrigDataAccessMonitoringLib TrigSerializeResultLib TrigNavigationLib TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigT1Result TrigCostMonitorLib )

# Install files from the package:
atlas_install_python_modules( python/TrigCost*.py )
atlas_install_joboptions( share/*.py )

