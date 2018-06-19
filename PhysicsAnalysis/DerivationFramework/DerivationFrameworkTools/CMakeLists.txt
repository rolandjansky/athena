################################################################################
# Package: DerivationFrameworkTools
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Event/xAOD/xAODBase
   GaudiKernel
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   Trigger/TrigAnalysis/TrigDecisionTool
   PRIVATE
   Control/AthenaKernel
   PhysicsAnalysis/CommonTools/ExpressionEvaluation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkToolsLib
   DerivationFrameworkTools/*.h src/*.cxx
   PUBLIC_HEADERS DerivationFrameworkTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps xAODBase GaudiKernel PATCoreLib
   TrigDecisionToolLib ExpressionEvaluationLib DerivationFrameworkInterfacesLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_component( DerivationFrameworkTools
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel DerivationFrameworkToolsLib )
