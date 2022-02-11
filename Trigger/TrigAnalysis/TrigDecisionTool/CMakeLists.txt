# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionTool )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )
set( ROOT_LIBRARIES_NOPYTHON ${ROOT_LIBRARIES} )
unset( ROOT_FOUND )
find_package( ROOT COMPONENTS Core PyROOT ROOTTPython )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TrigDecisionToolLib
      TrigDecisionTool/*.h Root/*.cxx
      PUBLIC_HEADERS TrigDecisionTool
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools CxxUtils xAODBase xAODTrigger
      TrigConfHLTData TrigConfL1Data TrigNavStructure TrigRoiConversionLib
      TrigConfInterfaces TrigDecisionInterface AsgDataHandlesLib
      TrigCompositeUtilsLib TrigConfHLTUtilsLib
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigSteeringEvent  )
else()
   if( XAOD_ANALYSIS )
      atlas_add_library( TrigDecisionToolLib
         TrigDecisionTool/*.h Root/*.cxx src/*.cxx
         PUBLIC_HEADERS TrigDecisionTool
         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools CxxUtils EventInfo xAODBase
         xAODTrigger GaudiKernel TrigConfHLTData TrigConfL1Data
         TrigNavStructure StoreGateLib TrigRoiConversionLib TrigCompositeUtilsLib
         TrigConfInterfaces TrigDecisionInterface TrigConfHLTUtilsLib
         PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AsgDataHandlesLib
         TrigSteeringEvent AthenaKernel )
   else()
      atlas_add_library( TrigDecisionToolLib
         TrigDecisionTool/*.h Root/*.cxx src/*.cxx
         PUBLIC_HEADERS TrigDecisionTool
         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools CxxUtils EventInfo xAODBase
         xAODTrigger GaudiKernel AnalysisTriggerEvent TrigConfHLTData
         TrigConfL1Data TrigDecisionEvent TrigMuonEvent TrigNavStructure
         TrigStorageDefinitions StoreGateLib TrigNavigationLib
         TrigRoiConversionLib TrigCompositeUtilsLib TrigConfInterfaces
         TrigDecisionInterface AsgDataHandlesLib TrigConfHLTUtilsLib
         PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES_NOPYTHON} AthenaBaseComps
         TrigSteeringEvent AthenaKernel )
   endif()

   atlas_add_component( TrigDecisionTool
      src/components/*.cxx
      LINK_LIBRARIES TrigDecisionToolLib )
endif()

atlas_add_dictionary( TrigDecisionToolDict
   TrigDecisionTool/TrigDecisionToolDict.h
   TrigDecisionTool/selection.xml
   LINK_LIBRARIES TrigDecisionToolLib ${ROOT_LIBRARIES} )

if( NOT XAOD_STANDALONE AND NOT XAOD_ANALYSIS )
   atlas_add_test( Templates_test
      SOURCES test/Templates_test.cxx
      LINK_LIBRARIES TrigNavigationLib AthenaKernel GaudiKernel AthContainers
      AnalysisTriggerEvent TrigSteeringEvent TrigMuonEvent
      TrigDecisionToolLib
      POST_EXEC_SCRIPT nopost.sh )
endif()

if (NOT XAOD_ANALYSIS )
   atlas_add_test( Run2TrigDecisionToolOnRAW_test 
      SCRIPT python -m TrigDecisionTool.TrigDecisionToolConfig --RAWRUN2
      LOG_SELECT_PATTERN "UnitTest")

      # AOD test
   atlas_add_test( Run2TrigDecisionToolOnAOD_test
      SCRIPT python -m TrigDecisionTool.TrigDecisionToolConfig
      LOG_SELECT_PATTERN "UnitTest")
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/checkTriggerEDM.py
   share/checkTriggerProcessingErrors.py )
atlas_install_joboptions( share/checkR3Trigger.py )
