################################################################################
# Package: ElectronPhotonSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonSelectorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PRIVATE
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint PyROOT )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( ElectronPhotonSelectorToolsLib
                   Root/*.cxx
                   PUBLIC_HEADERS ElectronPhotonSelectorTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODTracking PATCoreLib
                   PRIVATE_LINK_LIBRARIES FourMomUtils xAODCaloEvent GaudiKernel PathResolver )

atlas_add_component( ElectronPhotonSelectorTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODTracking PATCoreLib FourMomUtils xAODCaloEvent GaudiKernel PathResolver ElectronPhotonSelectorToolsLib )

atlas_add_dictionary( ElectronPhotonSelectorToolsDict
                      ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsCoreDict.h
                      ElectronPhotonSelectorTools/selectionCore.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODTracking PATCoreLib FourMomUtils xAODCaloEvent GaudiKernel PathResolver ElectronPhotonSelectorToolsLib )

atlas_add_dictionary( ElectronPhotonSelectorToolsPythonDict
                      ElectronPhotonSelectorTools/ElectronPhotonSelectorToolsPythonDict.h
                      ElectronPhotonSelectorTools/selectionPython.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODTracking PATCoreLib FourMomUtils xAODCaloEvent GaudiKernel PathResolver ElectronPhotonSelectorToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_data( data/*.conf )

