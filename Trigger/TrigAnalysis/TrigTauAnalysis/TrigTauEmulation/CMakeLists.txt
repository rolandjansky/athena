# $Id: CMakeLists.txt 770884 2016-08-30 18:04:17Z gbesjes $
################################################################################
# Package: TrigTauEmulation
################################################################################

# Declare the package name:
atlas_subdir( TrigTauEmulation )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODTau
  Event/xAOD/xAODTracking
  Event/xAOD/xAODTrigger
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  Trigger/TrigAnalysis/TrigDecisionTool
  PRIVATE
  Control/AthContainers
  Control/AthenaBaseComps
  Event/xAOD/xAODBase
  Event/xAOD/xAODCore
  Event/xAOD/xAODJet
  GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigTauEmulation
  TrigTauEmulation/*.h Root/*.cxx src/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTau
  xAODTracking xAODTrigger PATCoreLib PATInterfaces TrigDecisionToolLib
  AthContainers AthenaBaseComps xAODJet xAODBase xAODCore GaudiKernel )

atlas_add_dictionary( TrigTauEmulationDict
  TrigTauEmulation/TrigTauEmulationDict.h
  TrigTauEmulation/selection.xml
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTau
  xAODTracking xAODTrigger PATCoreLib PATInterfaces TrigDecisionToolLib
  AthContainers AthenaBaseComps xAODJet xAODBase xAODCore GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigTauEmulation )
atlas_install_python_modules( python/*.py )
