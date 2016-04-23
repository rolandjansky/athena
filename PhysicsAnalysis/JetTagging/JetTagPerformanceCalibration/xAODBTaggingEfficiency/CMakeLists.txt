# $Id: CMakeLists.txt 742367 2016-04-23 09:53:26Z krasznaa $
################################################################################
# Package: xAODBTaggingEfficiency
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingEfficiency )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBTagging
  Event/xAOD/xAODJet
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface
  PRIVATE
  Control/AthenaBaseComps
  GaudiKernel
  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( xAODBTaggingEfficiencyLib
  xAODBTaggingEfficiency/*.h Root/*.cxx
  PUBLIC_HEADERS xAODBTaggingEfficiency
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBTagging xAODJet
  PATCoreLib PATInterfaces CalibrationDataInterfaceLib PathResolver )

atlas_add_component( xAODBTaggingEfficiency
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES xAODJet CalibrationDataInterfaceLib AthenaBaseComps
  GaudiKernel PathResolver xAODBTaggingEfficiencyLib )

atlas_add_dictionary( xAODBTaggingEfficiencyDict
  xAODBTaggingEfficiency/xAODBTaggingEfficiencyDict.h
  xAODBTaggingEfficiency/selection.xml
  LINK_LIBRARIES xAODBTaggingEfficiencyLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root share/*.xml share/*.env )
