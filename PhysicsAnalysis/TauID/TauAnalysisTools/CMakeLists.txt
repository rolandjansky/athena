# $Id: CMakeLists.txt 796990 2017-02-14 07:47:22Z dduschin $
################################################################################
# Package: TauAnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( TauAnalysisTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Libraries in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TauAnalysisToolsLib
      TauAnalysisTools/*.h Root/*.cxx
	  PUBLIC_HEADERS TauAnalysisTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon xAODJet
      xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces
      tauRecToolsLib MCTruthClassifierLib
      PRIVATE_LINK_LIBRARIES AthLinks xAODEventInfo xAODMetaData PathResolver )
else()
   atlas_add_library( TauAnalysisToolsLib
      TauAnalysisTools/*.h Root/*.cxx
	  PUBLIC_HEADERS TauAnalysisTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon xAODJet
      xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces
      ElectronPhotonSelectorToolsLib tauRecToolsLib MCTruthClassifierLib
      PRIVATE_LINK_LIBRARIES AthLinks xAODEventInfo xAODMetaData PathResolver )
endif()

if( NOT XAOD_STANDALONE )
   atlas_add_component( TauAnalysisTools
      src/*.cxx src/components/*.cxx		     
      LINK_LIBRARIES AthenaBaseComps AsgTools xAODTau GaudiKernel
      TauAnalysisToolsLib )
endif()

atlas_add_dictionary( TauAnalysisToolsDict
   TauAnalysisTools/TauAnalysisToolsDict.h
   TauAnalysisTools/selection.xml
   LINK_LIBRARIES TauAnalysisToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( TauAnalysisToolsExample
      util/TauAnalysisToolsExample.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore xAODEventInfo
      xAODTau AsgTools PileupReweightingLib TauAnalysisToolsLib )
endif()

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( ut_TauAnalysisTools_test
      SOURCES test/ut_TauAnalysisTools_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccessLib
      GaudiKernel AsgTools xAODTau TauAnalysisToolsLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
