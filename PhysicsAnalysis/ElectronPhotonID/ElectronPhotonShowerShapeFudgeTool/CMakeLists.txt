################################################################################
# Package: ElectronPhotonShowerShapeFudgeTool
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonShowerShapeFudgeTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint PyROOT )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( ElectronPhotonShowerShapeFudgeToolLib
                   Root/*.cxx
                   PUBLIC_HEADERS ElectronPhotonShowerShapeFudgeTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma PATInterfaces
                   PRIVATE_LINK_LIBRARIES GaudiKernel PathResolver )

atlas_add_component( ElectronPhotonShowerShapeFudgeTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma PATInterfaces GaudiKernel PathResolver ElectronPhotonShowerShapeFudgeToolLib )

atlas_add_dictionary( ElectronPhotonShowerShapeFudgeToolDict
                      ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeToolDict.h
                      ElectronPhotonShowerShapeFudgeTool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma PATInterfaces GaudiKernel PathResolver ElectronPhotonShowerShapeFudgeToolLib )

