# $Id: CMakeLists.txt 740762 2016-04-16 08:04:59Z krasznaa $
################################################################################
# Package: ElectronPhotonSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonSelectorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODTracking
  PhysicsAnalysis/AnalysisCommon/PATCore
  PRIVATE
  Control/CxxUtils
  Event/FourMomUtils
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODCore
  GaudiKernel
  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist MathMore )

# Component(s) in the package:
atlas_add_library( ElectronPhotonSelectorToolsLib
  ElectronPhotonSelectorTools/*.h Root/*.cxx
  PUBLIC_HEADERS ElectronPhotonSelectorTools
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AsgTools xAODEgamma xAODTracking PATCoreLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils FourMomUtils
  xAODCaloEvent xAODCore GaudiKernel PathResolver )

atlas_add_component( ElectronPhotonSelectorTools
  src/components/*.cxx
  LINK_LIBRARIES ElectronPhotonSelectorToolsLib )

atlas_add_dictionary( ElectronPhotonSelectorToolsDict
  ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsCoreDict.h
  ElectronPhotonSelectorTools/selectionCore.xml
  LINK_LIBRARIES ElectronPhotonSelectorToolsLib )

atlas_add_dictionary( ElectronPhotonSelectorToolsPythonDict
  ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsPythonDict.h
  ElectronPhotonSelectorTools/selectionPython.xml
  LINK_LIBRARIES ElectronPhotonSelectorToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.root data/*.conf )
