################################################################################
# Package: DerivationFrameworkTools
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODBase
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Event/xAOD/xAODEventInfo
                          PRIVATE
                          Control/AthenaKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps xAODBase GaudiKernel PATCoreLib TrigDecisionToolLib ExpressionEvaluationLib xAODEventInfo
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_component( DerivationFrameworkTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODBase GaudiKernel PATCoreLib TrigDecisionToolLib AthenaKernel ExpressionEvaluationLib DerivationFrameworkToolsLib )

