# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
atlas_subdir( FakeBkgTools )

find_package( Boost )
find_package( ROOT COMPONENTS Core Tree RIO MathCore Gpad Hist MathMore Minuit Matrix )

atlas_add_root_dictionary( FakeBkgToolsLib FakeBkgToolsDictSource
   ROOT_HEADERS FakeBkgTools/TMinuit_LHMM.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( FakeBkgToolsLib
  FakeBkgTools/*.h Root/*.h Root/*.cxx  ${FakeBkgToolsDictSource}
  PUBLIC_HEADERS FakeBkgTools
  DEFINITIONS FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AsgAnalysisInterfaces AsgTools xAODBase PATInterfaces
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils xAODEgamma xAODMuon xAODTau PathResolver SelectionHelpersLib
  xAODCore xAODEventInfo )

if( NOT XAOD_STANDALONE )
  atlas_add_component( FakeBkgTools
    src/components/*.cxx
    LINK_LIBRARIES GaudiKernel FakeBkgToolsLib )
endif()

atlas_add_dictionary( FakeBkgToolsDict
   FakeBkgTools/FakeBkgToolsDict.h
   FakeBkgTools/selection.xml
   LINK_LIBRARIES FakeBkgToolsLib
)

set( pool_lib )
if( NOT XAOD_STANDALONE )
  set( pool_lib POOLRootAccessLib )
endif()

foreach( exec fbtNtupleTester fbtTestBasics fbtTestToyMC )
  atlas_add_executable( ${exec} util/${exec}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} FakeBkgToolsLib xAODEgamma xAODMuon ${pool_lib})
endforeach()

atlas_add_test( ut_fbtTestBasics
      SCRIPT $<TARGET_FILE:fbtTestBasics>
      POST_EXEC_SCRIPT nopost.sh
)
