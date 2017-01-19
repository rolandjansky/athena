# $Id: CMakeLists.txt 786880 2016-11-28 13:00:09Z krasznaa $
################################################################################
# Package: TrigDecisionTool
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionTool )

# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODBase
      Event/xAOD/xAODTrigger
      Trigger/TrigConfiguration/TrigConfHLTData
      Trigger/TrigConfiguration/TrigConfInterfaces
      Trigger/TrigConfiguration/TrigConfL1Data
      Trigger/TrigEvent/TrigDecisionInterface
      Trigger/TrigEvent/TrigRoiConversion
      Trigger/TrigEvent/TrigNavStructure
      PRIVATE
      Trigger/TrigEvent/TrigSteeringEvent )
elseif( XAOD_ANALYSIS )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Control/StoreGate
      Event/EventInfo
      Event/xAOD/xAODBase
      Event/xAOD/xAODTrigger
      GaudiKernel
      Trigger/TrigConfiguration/TrigConfHLTData
      Trigger/TrigConfiguration/TrigConfInterfaces
      Trigger/TrigConfiguration/TrigConfL1Data
      Trigger/TrigEvent/TrigDecisionInterface
      Trigger/TrigEvent/TrigRoiConversion
      Trigger/TrigEvent/TrigNavStructure
      PRIVATE
      Control/AthenaBaseComps
      Control/AthenaKernel
      Trigger/TrigEvent/TrigSteeringEvent )
else()
   atlas_depends_on_subdirs(
      PUBLIC
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
      Control/AthenaBaseComps
      Control/AthenaKernel
      Trigger/TrigEvent/TrigSteeringEvent )
endif()

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core PyROOT )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TrigDecisionToolLib
      TrigDecisionTool/*.h Root/*.cxx
      PUBLIC_HEADERS TrigDecisionTool
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools xAODBase xAODTrigger
      TrigConfHLTData TrigConfL1Data TrigNavStructure TrigRoiConversionLib
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigSteeringEvent )
else()
   if( XAOD_ANALYSIS )
      atlas_add_library( TrigDecisionToolLib
         TrigDecisionTool/*.h Root/*.cxx src/*.cxx
         PUBLIC_HEADERS TrigDecisionTool
         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools EventInfo xAODBase
         xAODTrigger GaudiKernel TrigConfHLTData TrigConfL1Data
         TrigNavStructure StoreGateLib TrigRoiConversionLib
         PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps
         TrigSteeringEvent AthenaKernel )
   else()
      atlas_add_library( TrigDecisionToolLib
         TrigDecisionTool/*.h Root/*.cxx src/*.cxx
         PUBLIC_HEADERS TrigDecisionTool
         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools EventInfo xAODBase
         xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData
         TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure
         TrigStorageDefinitions StoreGateLib TrigNavigationLib
         TrigRoiConversionLib
         PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps
         TrigSteeringEvent AthenaKernel )
   endif()

   atlas_add_component( TrigDecisionTool
      src/components/*.cxx
      LINK_LIBRARIES TrigDecisionToolLib )
endif()

atlas_add_dictionary( TrigDecisionToolDict
   TrigDecisionTool/TrigDecisionToolDict.h
   TrigDecisionTool/selection.xml
   LINK_LIBRARIES TrigDecisionToolLib )

if( NOT XAOD_STANDALONE AND NOT XAOD_ANALYSIS )
   atlas_add_test( Templates_test
      SOURCES test/Templates_test.cxx
      LINK_LIBRARIES TrigNavigationLib AthenaKernel GaudiKernel AthContainers
      AnalysisTriggerEvent TrigSteeringEvent TrigMuonEvent CxxUtils
      TrigDecisionToolLib
      EXTRA_PATTERNS ".*" )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/checkTriggerEDM.py
   share/checkTriggerProcessingErrors.py )
