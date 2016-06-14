################################################################################
# Package: DerivationFrameworkCalo
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Calorimeter/CaloClusterCorrection
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
                          Event/xAOD/xAODTau
                          Event/FourMomUtils
                          GaudiKernel
                          LArCalorimeter/LArTools
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkCaloLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkCalo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau CaloClusterCorrectionLib CaloUtilsLib LArToolsLib ExpressionEvaluationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloGeoHelpers FourMomUtils GaudiKernel )

atlas_add_component( DerivationFrameworkCalo
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau CaloClusterCorrectionLib CaloEvent CaloGeoHelpers CaloUtilsLib FourMomUtils GaudiKernel LArToolsLib ExpressionEvaluationLib DerivationFrameworkCaloLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
