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
atlas_add_library( TrigTauEmulationLib
   TrigTauEmulation/*.h Root/*.cxx
   PUBLIC_HEADERS TrigTauEmulation
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTau xAODTracking
   xAODTrigger PATCoreLib PATInterfaces TrigDecisionToolLib
   PRIVATE_LINK_LIBRARIES AthContainers xAODBase xAODCore )

atlas_add_component( TrigTauEmulation
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODJet xAODTau xAODTrigger
  xAODTracking TrigTauEmulationLib )

atlas_add_dictionary( TrigTauEmulationDict
  TrigTauEmulation/TrigTauEmulationDict.h
  TrigTauEmulation/selection.xml
  LINK_LIBRARIES AsgTools TrigTauEmulationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
