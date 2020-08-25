# $Id: CMakeLists.txt 780668 2016-10-27 10:10:01Z krasznaa $
################################################################################
# Package: xAODBTaggingEfficiency
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingEfficiency )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( xAODBTaggingEfficiencyLib
  xAODBTaggingEfficiency/*.h Root/*.cxx
  PUBLIC_HEADERS xAODBTaggingEfficiency
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBTagging xAODJet
  PATCoreLib PATInterfaces CalibrationDataInterfaceLib PathResolver
  FTagAnalysisInterfacesLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( xAODBTaggingEfficiency
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODJet CalibrationDataInterfaceLib AthenaBaseComps
      GaudiKernel PathResolver xAODBTaggingEfficiencyLib )
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
