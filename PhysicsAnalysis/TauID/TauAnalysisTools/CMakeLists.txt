# $Id: CMakeLists.txt 785220 2016-11-18 20:53:10Z dduschin $
################################################################################
# Package: TauAnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( TauAnalysisTools )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps GaudiKernel
      PhysicsAnalysis/POOLRootAccess
      Control/AthenaBaseComps
      Control/AthAnalysisBaseComps )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODMuon
   Event/xAOD/xAODJet
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Generators/TruthUtils
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/MCTruthClassifier
   Reconstruction/tauRecTools
   Tracking/TrkExtrapolation/TrkExUtils
   PRIVATE
   Control/AthLinks
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Libraries in the package:
atlas_add_library( TauAnalysisToolsLib
   TauAnalysisTools/*.h Root/*.cxx
	PUBLIC_HEADERS TauAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon xAODJet
   xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces TrkExUtils
   ElectronPhotonSelectorToolsLib tauRecToolsLib 
   InDetTrackSelectionToolLib MCTruthClassifierLib
   PRIVATE_LINK_LIBRARIES AthLinks xAODEventInfo PathResolver )

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
      xAODTau AsgTools  TauAnalysisToolsLib )
endif()

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( ut_TauAnalysisTools_test
      SOURCES test/ut_TauAnalysisTools_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccess
      GaudiKernel AsgTools xAODTau TauAnalysisToolsLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
