################################################################################
# Package: DerivationFrameworkCalo
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
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
   DerivationFrameworkCalo/*.h src/*.cxx
   PUBLIC_HEADERS DerivationFrameworkCalo
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES CaloIdentifier AthenaBaseComps CaloCalibHitRecLib xAODBase
   xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau CaloClusterCorrectionLib
   CaloUtilsLib LArToolsLib ExpressionEvaluationLib
   DerivationFrameworkInterfacesLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloGeoHelpers
   FourMomUtils GaudiKernel PathResolver )

atlas_add_component( DerivationFrameworkCalo
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel DerivationFrameworkCaloLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
