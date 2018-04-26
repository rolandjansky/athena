################################################################################
# Package: ElectronPhotonSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonSelectorTools )

# Extra dependencies based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODTracking
  Reconstruction/MVAUtils
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
  PRIVATE
  Control/CxxUtils
  Event/FourMomUtils
  Event/xAOD/xAODHIEvent
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODCore
  Event/xAOD/xAODEventInfo
  Tools/PathResolver
  ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist MathMore )

# Component(s) in the package:
atlas_add_library( ElectronPhotonSelectorToolsLib
  ElectronPhotonSelectorTools/*.h Root/*.cxx
  PUBLIC_HEADERS ElectronPhotonSelectorTools
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES EgammaAnalysisInterfacesLib AsgTools xAODEgamma  xAODTracking xAODHIEvent PATCoreLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils FourMomUtils
  xAODCaloEvent xAODCore xAODEventInfo PathResolver MVAUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronPhotonSelectorTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel ElectronPhotonSelectorToolsLib )
endif()

atlas_add_dictionary( ElectronPhotonSelectorToolsDict
  ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsCoreDict.h
  ElectronPhotonSelectorTools/selectionCore.xml
  LINK_LIBRARIES ElectronPhotonSelectorToolsLib )

atlas_add_dictionary( ElectronPhotonSelectorToolsPythonDict
  ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsPythonDict.h
  ElectronPhotonSelectorTools/selectionPython.xml
  LINK_LIBRARIES ElectronPhotonSelectorToolsLib )

atlas_add_executable( EGIdentification_mem_check
   util/EGIdentification_mem_check.cxx
   LINK_LIBRARIES CxxUtils ElectronPhotonSelectorToolsLib )

if( XAOD_STANDALONE )

   atlas_add_executable( EGIdentification_testEGIdentificationPoints
      util/testEGIdentificationPoints.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCore xAODEgamma CxxUtils AsgTools
      xAODRootAccess ElectronPhotonSelectorToolsLib )

   atlas_add_executable( EGIdentification_testEGChargeIDSelector
      util/testEGChargeIDSelector.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma AsgTools xAODRootAccess 
      ElectronPhotonSelectorToolsLib )

endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.root data/*.conf )
