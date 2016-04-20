################################################################################
# Package: DerivationFrameworkEGamma
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkEGamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
                          PRIVATE
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/MCTruthClassifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkEGammaLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkEGamma
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps PATCoreLib ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib ExpressionEvaluationLib MCTruthClassifierLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODCaloEvent xAODEgamma xAODMissingET xAODMuon xAODTruth GaudiKernel )

atlas_add_component( DerivationFrameworkEGamma
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps PATCoreLib ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib xAODBase xAODCaloEvent xAODEgamma xAODMissingET xAODMuon xAODTruth GaudiKernel ExpressionEvaluationLib MCTruthClassifierLib DerivationFrameworkEGammaLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

