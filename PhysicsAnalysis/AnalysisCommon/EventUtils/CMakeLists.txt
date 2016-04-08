################################################################################
# Package: EventUtils
################################################################################

# Declare the package name:
atlas_subdir( EventUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Event/EventInfo
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EventUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaBaseComps CxxUtils EventInfo xAODBTagging xAODBase xAODCaloEvent xAODCore xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth GaudiKernel PATCoreLib ExpressionEvaluationLib TrigDecisionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

