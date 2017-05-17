# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GoodRunsLists )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree TreePlayer XMLIO XMLParser )
find_package( LibXml2 )

# Component(s) in the package:
atlas_add_root_dictionary( GoodRunsListsLib
   GoodRunsListsLibDictSource
   ROOT_HEADERS GoodRunsLists/TLumiBlockRange.h GoodRunsLists/RegularFormula.h
   GoodRunsLists/TGoodRunsListReader.h GoodRunsLists/TGoodRunsListWriter.h
   GoodRunsLists/DQHelperFunctions.h Root/LinkDef.h
   EXTERNAL_PACKAGES LibXml2 ROOT )

atlas_add_library( GoodRunsListsLib
   GoodRunsLists/*.h Root/*.cxx ${GoodRunsListsLibDictSource}
   PUBLIC_HEADERS GoodRunsLists
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AsgAnalysisInterfaces
   PRIVATE_LINK_LIBRARIES ${LIBXML2_LIBRARIES} PathResolver )

if( NOT XAOD_STANDALONE )
   if( XAOD_ANALYSIS )
     set( extra_libs )
   else()
     set( extra_libs DetectorStatusLib )
   endif()
   atlas_add_component( GoodRunsLists
      src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps AthenaKernel EventInfo GaudiKernel GoodRunsListsLib PathResolver StoreGateLib xAODEventInfo )
endif()

atlas_add_dictionary( GoodRunsListsDict
   GoodRunsLists/GoodRunsListsDict.h
   GoodRunsLists/selection.xml
   LINK_LIBRARIES GoodRunsListsLib )

# Helper variable for setting up the test(s):
set( testLibs )
if( NOT XAOD_STANDALONE )
   set( testLibs TestTools )
endif()

# Test(s) in the package:
atlas_add_test( GRLSelectionTool_test
   SOURCES test/GRLSelectionTool_test.cxx
   LINK_LIBRARIES GoodRunsListsLib ${testLibs}
   PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/define* scripts/create* scripts/merge*
   scripts/verify* scripts/overlap* scripts/diff* )
atlas_install_xmls(
   test/data12_8TeV.periodAllYear_DetStatus-v54-pro13-04_DQDefects-00-00-33_PHYS_StandardGRL_All_Good.xml )
