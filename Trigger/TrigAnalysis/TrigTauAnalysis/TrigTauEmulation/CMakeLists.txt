# $Id: CMakeLists.txt 740790 2016-04-16 14:46:57Z krasznaa $
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
  AthContainers AthenaBaseComps xAODJet GaudiKernel )

atlas_add_dictionary( TrigTauEmulationDict
  TrigTauEmulation/TrigTauEmulationDict.h
  TrigTauEmulation/selection.xml
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTau
  xAODTracking xAODTrigger PATCoreLib PATInterfaces TrigDecisionToolLib
  AthContainers AthenaBaseComps xAODJet GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigTauEmulation )
atlas_install_python_modules( python/*.py )
