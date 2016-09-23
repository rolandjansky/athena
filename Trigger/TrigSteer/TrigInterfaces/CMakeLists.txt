################################################################################
# Package: TrigInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/DataModel
                          GaudiKernel
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigStorageDefinitions
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/xAOD/xAODTrigger
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_library( TrigInterfacesLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigInterfaces
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps DataModel GaudiKernel TrigSteeringEvent TrigStorageDefinitions AthenaMonitoringLib TrigNavigationLib StoreGateLib SGtests TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES TestTools AthenaKernel xAODTrigger )

atlas_add_component( TrigInterfaces
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib DataModel GaudiKernel TrigNavigationLib TrigSteeringEvent TrigStorageDefinitions TestTools AthenaKernel StoreGateLib SGtests xAODTrigger TrigTimeAlgsLib TrigInterfacesLib )

atlas_add_test( Templates_test
                SOURCES
                test/Templates_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib DataModel GaudiKernel TrigNavigationLib TrigSteeringEvent TrigStorageDefinitions TestTools AthenaKernel StoreGateLib SGtests xAODTrigger TrigTimeAlgsLib TrigInterfacesLib
                EXTRA_PATTERNS ".*" )

atlas_add_test( MonitoredAlgo_test
                SOURCES
                test/MonitoredAlgo_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib DataModel GaudiKernel TrigNavigationLib TrigSteeringEvent TrigStorageDefinitions TestTools AthenaKernel StoreGateLib SGtests xAODTrigger TrigTimeAlgsLib TrigInterfacesLib
                EXTRA_PATTERNS ".*" )

atlas_add_test( HLTCheck_test
                SOURCES
                test/HLTCheck_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaMonitoringLib DataModel GaudiKernel TrigNavigationLib TrigSteeringEvent TrigStorageDefinitions TestTools AthenaKernel StoreGateLib SGtests xAODTrigger TrigTimeAlgsLib TrigInterfacesLib
                EXTRA_PATTERNS ".*" )

