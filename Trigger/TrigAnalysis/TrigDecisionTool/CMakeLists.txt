# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionTool )

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
      TrigConfHLTData TrigConfL1Data TrigNavStructure TrigRoiConversionLib TrigConfInterfaces
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
         TrigNavStructure StoreGateLib TrigRoiConversionLib TrigCompositeUtilsLib  TrigConfInterfaces
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
         TrigRoiConversionLib TrigCompositeUtilsLib TrigConfInterfaces
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
      AnalysisTriggerEvent TrigSteeringEvent TrigMuonEvent 
      TrigDecisionToolLib
      POST_EXEC_SCRIPT nopost.sh )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/checkTriggerEDM.py
   share/checkTriggerProcessingErrors.py )
atlas_install_joboptions( share/checkR3Trigger.py )

