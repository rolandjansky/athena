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
                          PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          PRIVATE
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/MCTruthClassifier 
			  Calorimeter/CaloCalibHitRec )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkEGammaLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkEGamma
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps PATCoreLib EgammaAnalysisInterfacesLib ExpressionEvaluationLib MCTruthClassifierLib CaloCalibHitRecLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODCaloEvent xAODEgamma xAODMissingET xAODMuon xAODTruth )

atlas_add_component( DerivationFrameworkEGamma
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES}  AsgTools AthenaBaseComps PATCoreLib  EgammaAnalysisInterfacesLib xAODBase 
		     xAODCaloEvent xAODEgamma xAODMissingET xAODMuon xAODTruth GaudiKernel ExpressionEvaluationLib MCTruthClassifierLib CaloCalibHitRecLib DerivationFrameworkEGammaLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

