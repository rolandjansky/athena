################################################################################
# Package: ExpressionEvaluation
################################################################################

# Declare the package name:
atlas_subdir( ExpressionEvaluation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Generators/TruthUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ExpressionEvaluationLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS ExpressionEvaluation
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools GaudiKernel StoreGateLib SGtests TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} xAODBTagging xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODEventShape xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth TruthUtils )

atlas_add_component( ExpressionEvaluation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthContainers SGTools StoreGateLib SGtests GaudiKernel TrigDecisionToolLib xAODBTagging xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODEventShape xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth TruthUtils ExpressionEvaluationLib )

