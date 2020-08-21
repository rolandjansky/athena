################################################################################
# Package: EventUtils
################################################################################

# Declare the package name:
atlas_subdir( EventUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EventUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthAnalysisBaseCompsLib AthContainers AthLinks AthenaBaseComps AthenaKernel CxxUtils EventInfo xAODBTagging xAODBase xAODCaloEvent xAODCore xAODCutFlow xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth GaudiKernel PATCoreLib ExpressionEvaluationLib TrigDecisionToolLib DerivationFrameworkInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

