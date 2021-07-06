# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigGlobalEfficiencyCorrection )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS RIO )

# Component(s) in the package:
atlas_add_library( TrigGlobalEfficiencyCorrectionLib
   TrigGlobalEfficiencyCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS TrigGlobalEfficiencyCorrection
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AsgMessagingLib AsgTools AthContainers EgammaAnalysisInterfacesLib MuonAnalysisInterfacesLib TriggerAnalysisInterfaces TriggerMatchingToolLib xAODBase xAODEgamma
   PRIVATE_LINK_LIBRARIES PathResolver xAODEventInfo )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigGlobalEfficiencyCorrection
      src/components/*.cxx
      LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib )
endif()

atlas_add_dictionary( TrigGlobalEfficiencyCorrectionDict
   TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionDict.h
   TrigGlobalEfficiencyCorrection/selection.xml
   LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib )

set( pool_lib )
if( NOT XAOD_STANDALONE )
  set( pool_lib POOLRootAccessLib AthAnalysisBaseCompsLib )
endif()

foreach( example TrigGlobEffCorrExample0 TrigGlobEffCorrExample1
      TrigGlobEffCorrExample3a TrigGlobEffCorrExample3b TrigGlobEffCorrExample3c
      TrigGlobEffCorrExample3d TrigGlobEffCorrExample3e TrigGlobEffCorrExample4
      TrigGlobEffCorrExample5a TrigGlobEffCorrExample5b TrigGlobEffCorrExample6 )
   atlas_add_executable( ${example}
      SOURCES examples/${example}.cxx
	  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	  LINK_LIBRARIES ${ROOT_LIBRARIES} TrigGlobalEfficiencyCorrectionLib ${pool_lib} )
endforeach()

atlas_add_executable( TrigGlobEffCorrValidation
   SOURCES util/TrigGlobEffCorrValidation.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils TrigGlobalEfficiencyCorrectionLib xAODRootAccess )

atlas_install_data( data/*.cfg )

# Test(s) in the package:
atlas_add_test( Tool
   SCRIPT TrigGlobEffCorrValidation --quiet --fast --skip-if-athena
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( CPTools
   SCRIPT TrigGlobEffCorrExample4 --quiet
   POST_EXEC_SCRIPT nopost.sh )
