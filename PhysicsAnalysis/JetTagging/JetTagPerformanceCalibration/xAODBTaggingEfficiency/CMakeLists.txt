# $Id: CMakeLists.txt 780668 2016-10-27 10:10:01Z krasznaa $
################################################################################
# Package: xAODBTaggingEfficiency
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingEfficiency )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
   set( extra_libs xAODRootAccess )
else()
   if ( XAOD_ANALYSIS AND NOT ROOTCORE )
      set( extra_deps Control/AthenaBaseComps PRIVATE PhysicsAnalysis/POOLRootAccess GaudiKernel )
      set( extra_libs AthAnalysisBaseCompsLib )
   else()
      set( extra_deps Control/AthenaBaseComps GaudiKernel )
   endif()
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBTagging
  Event/xAOD/xAODJet
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  PhysicsAnalysis/Interfaces/FTagAnalysisInterfaces
  PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface
  ${extra_deps}
  PRIVATE
  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

atlas_add_root_dictionary( xAODBTaggingEfficiencyLib xAODBTaggingEfficiencyLibDictSource ROOT_HEADERS xAODBTaggingEfficiency/TruthTagResults.h Root/LinkDef.h )

# Component(s) in the package:
atlas_add_library( xAODBTaggingEfficiencyLib
  ${xAODBTaggingEfficiencyLibDictSource}
  xAODBTaggingEfficiency/*.h Root/*.cxx
  PUBLIC_HEADERS xAODBTaggingEfficiency
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBTagging xAODJet
  PATCoreLib PATInterfaces CalibrationDataInterfaceLib PathResolver FTagAnalysisInterfacesLib ${extra_libs} )


if( NOT XAOD_STANDALONE )
   atlas_add_component( xAODBTaggingEfficiency
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODJet CalibrationDataInterfaceLib AthenaBaseComps
      GaudiKernel PathResolver xAODBTaggingEfficiencyLib FTagAnalysisInterfacesLib )
endif()

atlas_add_dictionary( xAODBTaggingEfficiencyDict
  xAODBTaggingEfficiency/xAODBTaggingEfficiencyDict.h
  xAODBTaggingEfficiency/selection.xml
  LINK_LIBRARIES xAODBTaggingEfficiencyLib )

# Executable(s) in the package (to be built only under AthAnalysis or in stand-alone mode):
if( XAOD_ANALYSIS OR XAOD_STANDALONE )
   atlas_add_executable( BTaggingEfficiencyToolTester
      util/BTaggingEfficiencyToolTester.cxx
      LINK_LIBRARIES xAODRootAccess xAODBTaggingEfficiencyLib )
   atlas_add_executable( BTaggingEigenVectorRecompositionToolTester
      util/BTaggingEigenVectorRecompositionToolTester.cxx
      LINK_LIBRARIES xAODRootAccess xAODBTaggingEfficiencyLib )
endif()

if ( XAOD_ANALYSIS AND NOT ROOTCORE )
   atlas_add_executable( BTaggingSelectionToolTester
      util/BTaggingSelectionToolTester.cxx
      LINK_LIBRARIES xAODJet xAODBTagging xAODBTaggingEfficiencyLib POOLRootAccessLib GaudiKernel )
elseif( XAOD_STANDALONE OR ROOTCORE )
      atlas_add_executable( BTaggingSelectionToolTester
      util/BTaggingSelectionToolTester.cxx
      LINK_LIBRARIES xAODJet xAODBTagging xAODBTaggingEfficiencyLib xAODRootAccess )
endif()


if( XAOD_ANALYSIS OR XAOD_STANDALONE )
   atlas_add_executable( BTaggingTruthTaggingTester
      util/BTaggingTruthTaggingTester.cxx
      LINK_LIBRARIES xAODJet xAODBTagging xAODBTaggingEfficiencyLib xAODRootAccess FTagAnalysisInterfacesLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root share/*.xml share/*.env )
