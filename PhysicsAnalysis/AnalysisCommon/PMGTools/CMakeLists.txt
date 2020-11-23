# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PMGTools )

# Extra libraries based on the build environment:
set( xaod_access_lib )
set( extra_private_libs )
# ... for AnalysisBase
if( XAOD_STANDALONE )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( xaod_access_lib POOLRootAccessLib )
   set( extra_private_libs AthAnalysisBaseCompsLib EventInfo )
endif()

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( Boost )

# Libraries in the package:
atlas_add_library( PMGToolsLib
   PMGTools/*.h Root/*.cxx
   PUBLIC_HEADERS PMGTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATCoreLib PATInterfaces PMGAnalysisInterfacesLib xAODTruth
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils FourMomUtils PathResolver RootCoreUtils xAODBase xAODEventInfo xAODJet xAODMetaData ${extra_private_libs})

if( NOT XAOD_STANDALONE )
   atlas_add_component( PMGTools
      src/components/*.cxx
      LINK_LIBRARIES PMGToolsLib )
endif()

atlas_add_dictionary( PMGToolsDict
   PMGTools/PMGToolsDict.h
   PMGTools/selection.xml
   LINK_LIBRARIES PMGToolsLib )

# Executable(s) in the package:
#
# ... for Athena/AthAnalysis
if( NOT XAOD_STANDALONE )
   atlas_add_executable( MyPMGApp
      test/MyPMGApp.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools GaudiKernel PATInterfaces
      PMGAnalysisInterfacesLib PMGToolsLib xAODJet xAODTruth ${xaod_access_lib} )
endif()

# Test(s) in the package:
#
# ... for AnalysisBase
if( XAOD_STANDALONE )
   atlas_add_test( ut_PMGSherpa22VJetsWeightTool_test
      SOURCES test/ut_PMGSherpa22VJetsWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGToolsLib ${xaod_access_lib} )

   atlas_add_test( ut_PMGSherpaVjetsSysTool_test
      SOURCES test/ut_PMGSherpaVjetsSysTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGAnalysisInterfacesLib PMGToolsLib ${xaod_access_lib} )
endif()
# ... AthAnalysis/AnalysisBase
atlas_add_test( ut_PMGTruthWeightTool_test
   SOURCES test/ut_PMGTruthWeightTool_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools PMGAnalysisInterfacesLib PMGToolsLib xAODTruth ${xaod_access_lib} )
