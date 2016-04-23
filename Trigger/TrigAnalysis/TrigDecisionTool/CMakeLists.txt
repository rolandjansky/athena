################################################################################
# Package: TrigDecisionTool
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigDecisionEvent
                          Trigger/TrigEvent/TrigDecisionInterface
                          Trigger/TrigEvent/TrigMuonEvent
			  Trigger/TrigEvent/TrigRoiConversion
                          Trigger/TrigEvent/TrigNavStructure
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigStorageDefinitions
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigDecisionToolLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrigDecisionTool
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools EventInfo xAODBase xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure TrigStorageDefinitions StoreGateLib SGtests TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthenaBaseComps TrigSteeringEvent )

atlas_add_component( TrigDecisionTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools StoreGateLib SGtests EventInfo xAODBase xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigStorageDefinitions TestTools AthenaBaseComps TrigSteeringEvent TrigDecisionToolLib )

atlas_add_dictionary( TrigDecisionToolDict
                      TrigDecisionTool/TrigDecisionToolDict.h
                      TrigDecisionTool/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools StoreGateLib SGtests EventInfo xAODBase xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigStorageDefinitions TestTools AthenaBaseComps TrigSteeringEvent TrigDecisionToolLib )

atlas_add_test( Templates_test
                SOURCES
                test/Templates_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools StoreGateLib SGtests EventInfo xAODBase xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigStorageDefinitions TestTools AthenaBaseComps TrigSteeringEvent TrigDecisionToolLib
                EXTRA_PATTERNS ".*" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/checkTriggerEDM.py share/checkTriggerProcessingErrors.py )

