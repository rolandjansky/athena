# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HFORTools )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core RIO )

# Libraries in the package:
atlas_add_library( HFORToolsLib
   HFORTools/*.h Root/*.cxx
   PUBLIC_HEADERS HFORTools
   LINK_LIBRARIES AsgTools PMGAnalysisInterfacesLib xAODTruth xAODJet
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} PathResolver xAODBase xAODEventInfo
   xAODRootAccess )

if( NOT XAOD_STANDALONE )
   atlas_add_component( HFORTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODEventInfo PMGAnalysisInterfacesLib HFORToolsLib )
endif()

atlas_add_dictionary( HFORToolsDict
   HFORTools/HFORToolsDict.h
   HFORTools/selection.xml
   LINK_LIBRARIES HFORToolsLib )

# Figure out which library to use for xAOD reading in the test(s).
set( test_lib )
if( XAOD_STANDALONE )
   set( test_lib xAODRootAccess ${ROOT_LIBRARIES}
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )
else()
   set( test_lib POOLRootAccessLib )
endif()

# Test(s) in the package:
atlas_add_test( ut_HFORTools_test
   SOURCES test/ut_HFORTools_test.cxx
   LINK_LIBRARIES AsgTools PMGAnalysisInterfacesLib xAODEventInfo HFORToolsLib ${test_lib}
   POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_data( data/*.cfg )
