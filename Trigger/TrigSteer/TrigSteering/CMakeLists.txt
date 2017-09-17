################################################################################
# Package: TrigSteering
################################################################################

# Declare the package name:
atlas_subdir( TrigSteering )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Control/DataModel
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigDataAccess/TrigROBDataProviderSvc
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/L1Topo/L1TopoAlgorithms
                          Trigger/TrigT1/L1Topo/L1TopoCoreSim
                          Trigger/TrigT1/L1Topo/L1TopoEvent
                          Trigger/TrigT1/L1Topo/L1TopoSimulation
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1Result
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          AtlasTest/TestTools
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigDataAccess/TrigSerializeResult
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigStorageDefinitions
                          Trigger/TrigMonitoring/TrigMonitorBase
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/L1Topo/L1TopoCommon
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# tag NEEDS_COOL_FACTORY was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrigSteeringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSteering
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel DataModel EventInfo xAODEventInfo GaudiKernel TrigConfBase TrigConfHLTData TrigSteeringEvent L1TopoAlgorithms L1TopoCoreSim L1TopoEvent TrigT1CaloToolInterfaces TrigT1Result AthenaMonitoringLib StoreGateLib SGtests TrigROBDataProviderSvcLib L1TopoSimulationLib TrigT1CaloEventLib TrigTimeAlgsLib ByteStreamCnvSvcBaseLib TrigSerializeResultLib TrigNavigationLib TrigMonitorBaseLib TrigInterfacesLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} TestTools xAODTrigger TrigConfL1Data TrigStorageDefinitions L1TopoCommon L1TopoConfig TrigT1Interfaces )

atlas_add_component( TrigSteering
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib DataModel StoreGateLib SGtests EventInfo xAODEventInfo GaudiKernel TrigConfBase TrigConfHLTData TrigROBDataProviderSvcLib TrigSteeringEvent L1TopoAlgorithms L1TopoCoreSim L1TopoEvent L1TopoSimulationLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Result TrigTimeAlgsLib TestTools ByteStreamCnvSvcBaseLib xAODTrigger TrigConfL1Data TrigSerializeResultLib TrigNavigationLib TrigStorageDefinitions TrigMonitorBaseLib TrigInterfacesLib L1TopoCommon L1TopoConfig TrigT1Interfaces TrigSteeringLib )

atlas_add_dictionary( TrigSteeringDict
                      TrigSteering/TrigSteeringDict.h
                      TrigSteering/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib DataModel StoreGateLib SGtests EventInfo xAODEventInfo GaudiKernel TrigConfBase TrigConfHLTData TrigROBDataProviderSvcLib TrigSteeringEvent L1TopoAlgorithms L1TopoCoreSim L1TopoEvent L1TopoSimulationLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Result TrigTimeAlgsLib TestTools ByteStreamCnvSvcBaseLib xAODTrigger TrigConfL1Data TrigSerializeResultLib TrigNavigationLib TrigStorageDefinitions TrigMonitorBaseLib TrigInterfacesLib L1TopoCommon L1TopoConfig TrigT1Interfaces TrigSteeringLib )

atlas_add_test( Signature_test
                SOURCES
                test/Signature_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib DataModel StoreGateLib SGtests EventInfo xAODEventInfo GaudiKernel TrigConfBase TrigConfHLTData TrigROBDataProviderSvcLib TrigSteeringEvent L1TopoAlgorithms L1TopoCoreSim L1TopoEvent L1TopoSimulationLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Result TrigTimeAlgsLib TestTools ByteStreamCnvSvcBaseLib xAODTrigger TrigConfL1Data TrigSerializeResultLib TrigNavigationLib TrigStorageDefinitions TrigMonitorBaseLib TrigInterfacesLib L1TopoCommon L1TopoConfig TrigT1Interfaces TrigSteeringLib
                EXTRA_PATTERNS ".*" )


atlas_add_test( SteeringChain_test
                SOURCES
                test/SteeringChain_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib DataModel StoreGateLib SGtests EventInfo xAODEventInfo GaudiKernel TrigConfBase TrigConfHLTData TrigROBDataProviderSvcLib TrigSteeringEvent L1TopoAlgorithms L1TopoCoreSim L1TopoEvent L1TopoSimulationLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Result TrigTimeAlgsLib TestTools ByteStreamCnvSvcBaseLib xAODTrigger TrigConfL1Data TrigSerializeResultLib TrigNavigationLib TrigStorageDefinitions TrigMonitorBaseLib TrigInterfacesLib L1TopoCommon L1TopoConfig TrigT1Interfaces TrigSteeringLib
                EXTRA_PATTERNS ".*" )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/Lvl1Results.txt )
atlas_install_xmls( share/*.dtd )

