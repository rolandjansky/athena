# $Id: CMakeLists.txt 777875 2016-10-11 17:17:13Z krasznaa $
################################################################################
# Package: GoodRunsLists
################################################################################

# Declare the package name:
atlas_subdir( GoodRunsLists )

# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODEventInfo
      PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
      PRIVATE
      Tools/PathResolver )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Control/AthenaBaseComps
      Control/AthenaKernel
      Event/xAOD/xAODEventInfo
      PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
      GaudiKernel
      PRIVATE
      Control/StoreGate
      DetectorDescription/DetDescrCond/DetectorStatus
      Event/EventInfo
      Tools/PathResolver
      AtlasTest/TestTools )
endif()

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
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo AsgAnalysisInterfaces
   PRIVATE_LINK_LIBRARIES ${LIBXML2_LIBRARIES} PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( GoodRunsLists
      src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps AthenaKernel xAODEventInfo GaudiKernel
      StoreGateLib DetectorStatusLib EventInfo PathResolver GoodRunsListsLib )
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
   LINK_LIBRARIES GoodRunsListsLib ${testLibs} )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/define* scripts/create* scripts/merge*
   scripts/verify* scripts/overlap* scripts/diff* )
atlas_install_xmls(
   test/data12_8TeV.periodAllYear_DetStatus-v54-pro13-04_DQDefects-00-00-33_PHYS_StandardGRL_All_Good.xml )
