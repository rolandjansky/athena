# $Id: CMakeLists.txt 796990 2017-02-14 07:47:22Z dduschin $
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
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/MCTruthClassifier
   Reconstruction/tauRecTools
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PRIVATE
   Control/AthLinks
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMetaData
   Tools/PathResolver
   PhysicsAnalysis/AnalysisCommon/PileupReweighting
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Libraries in the package:
atlas_add_library( TauAnalysisToolsLib
   TauAnalysisTools/*.h Root/*.cxx
	PUBLIC_HEADERS TauAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon xAODJet
   xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces
   ElectronPhotonSelectorToolsLib tauRecToolsLib MCTruthClassifierLib
   PRIVATE_LINK_LIBRARIES AthLinks xAODEventInfo xAODMetaData PathResolver )

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
   atlas_add_executable( TauAnalysisToolsValidation
      util/TauAnalysisToolsValidation.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore xAODEventInfo
      xAODTau AsgTools PileupReweightingLib TauAnalysisToolsLib )
   atlas_add_executable( DiTauAnalysisExample
      util/DiTauAnalysisExample.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore xAODEventInfo
      xAODTau AsgTools tauRecToolsLib TauAnalysisToolsLib )
   atlas_add_executable( TauAnalysisToolsValidation
      util/TauAnalysisToolsValidation.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore xAODEventInfo
      xAODTau AsgTools tauRecToolsLib TauAnalysisToolsLib )
endif()

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( ut_TauAnalysisTools_test
      SOURCES test/ut_TauAnalysisTools_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccessLib
      GaudiKernel AsgTools xAODTau TauAnalysisToolsLib )
else()
atlas_add_test( ut_TauCorrections_test
   SOURCES test/ut_TauCorrections_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCore
   xAODTau AsgTools TauAnalysisToolsLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
