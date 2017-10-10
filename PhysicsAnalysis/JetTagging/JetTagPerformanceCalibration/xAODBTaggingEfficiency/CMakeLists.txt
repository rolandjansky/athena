# $Id: CMakeLists.txt 780668 2016-10-27 10:10:01Z krasznaa $
################################################################################
# Package: xAODBTaggingEfficiency
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingEfficiency )

# Extra dependencies based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBTagging
  Event/xAOD/xAODJet
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  PhysicsAnalysis/Intefaces/FTagAnalysisInterfaces
  PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface
  PRIVATE
  Tools/PathResolver
  ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( xAODBTaggingEfficiencyLib
  xAODBTaggingEfficiency/*.h Root/*.cxx
  PUBLIC_HEADERS xAODBTaggingEfficiency
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBTagging xAODJet
  PATCoreLib PATInterfaces CalibrationDataInterfaceLib PathResolver FTagAnalysisInterfacesLib )

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

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( BTaggingEfficiencyToolTester
      util/BTaggingEfficiencyToolTester.cxx
      LINK_LIBRARIES xAODRootAccess xAODBTaggingEfficiencyLib )

   atlas_add_executable( BTaggingSelectionToolTester
      util/BTaggingSelectionToolTester.cxx
      LINK_LIBRARIES xAODJet xAODBTagging xAODBTaggingEfficiencyLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root share/*.xml share/*.env )
