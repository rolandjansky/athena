################################################################################
# Package: RingerSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( RingerSelectorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/xAOD/xAODCaloRings
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PRIVATE
                          Control/AthContainers
                          Event/xAOD/xAODBase
                          GaudiKernel
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkTrackSummary )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( RingerSelectorToolsLib
                   Root/*.cxx
                   Root/procedures/*.cxx
                   Root/tools/*.cxx
                   PUBLIC_HEADERS RingerSelectorTools
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaKernel CxxUtils xAODCaloRings xAODEgamma xAODTracking PATCoreLib ElectronPhotonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES AthContainers xAODBase GaudiKernel PathResolver TrkTrackSummary )

atlas_add_component( RingerSelectorTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaKernel CxxUtils xAODCaloRings xAODEgamma xAODTracking PATCoreLib ElectronPhotonSelectorToolsLib AthContainers xAODBase GaudiKernel PathResolver TrkTrackSummary RingerSelectorToolsLib )

atlas_add_dictionary( RingerSelectorToolsDict
                      RingerSelectorTools/RingerSelectorToolsDict.h
                      RingerSelectorTools/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaKernel CxxUtils xAODCaloRings xAODEgamma xAODTracking PATCoreLib ElectronPhotonSelectorToolsLib AthContainers xAODBase GaudiKernel PathResolver TrkTrackSummary RingerSelectorToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
