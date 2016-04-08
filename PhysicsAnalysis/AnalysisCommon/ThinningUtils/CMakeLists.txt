################################################################################
# Package: ThinningUtils
################################################################################

# Declare the package name:
atlas_subdir( ThinningUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventInfo
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces )

# Component(s) in the package:
atlas_add_component( ThinningUtils
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent TrigDecisionToolLib AthLinks AthenaBaseComps AthenaKernel EventInfo xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODParticleEvent xAODTau xAODTracking GaudiKernel ExpressionEvaluationLib )

# Install files from the package:
atlas_install_headers( ThinningUtils )
atlas_install_python_modules( python/*.py )

