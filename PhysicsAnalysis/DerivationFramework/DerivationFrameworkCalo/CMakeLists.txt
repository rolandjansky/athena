################################################################################
# Package: DerivationFrameworkCalo
################################################################################
 
# Declare the package name:
atlas_subdir( DerivationFrameworkCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
			  Reconstruction/egamma/egammaInterfaces
			  Calorimeter/CaloCalibHitRec
                          PRIVATE
                          Calorimeter/CaloClusterCorrection
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
			  Control/CxxUtils
			  Event/xAOD/xAODTau
    			  Event/xAOD/xAODMuon
                          Event/xAOD/xAODBase
                          Event/FourMomUtils
                          GaudiKernel
                          LArCalorimeter/LArTools
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
		          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkCaloLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkCalo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps CaloCalibHitRecLib xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau CaloClusterCorrectionLib CaloUtilsLib LArToolsLib ExpressionEvaluationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloGeoHelpers FourMomUtils GaudiKernel PathResolver)

atlas_add_component( DerivationFrameworkCalo
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier CaloCalibHitRecLib AthenaBaseComps xAODBase
                     xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau CaloClusterCorrectionLib CaloEvent CaloGeoHelpers CaloUtilsLib FourMomUtils GaudiKernel LArToolsLib ExpressionEvaluationLib DerivationFrameworkCaloLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
