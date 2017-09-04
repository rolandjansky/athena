################################################################################
# Package: DerivationFrameworkSUSY
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkSUSY )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Reconstruction/RecoTools/RecoToolInterfaces
			  Tracking/TrkEvent/VxVertex
			  Tracking/TrkEvent/TrkTrack
                          PhysicsAnalysis/AnalysisCommon/ReweightUtils
                          PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODAssociations
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
			  Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
			  Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
			  Event/EventPrimitives
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier
			  Event/xAOD/xAODParticleEvent )


# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkSUSYLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkSUSY
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps xAODTruth RecoToolInterfaces ExpressionEvaluationLib MCTruthClassifierLib ReweightUtilsLib TrkExUtils
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel xAODAssociations xAODEventInfo xAODJet xAODTracking GaudiKernel )

atlas_add_component( DerivationFrameworkSUSY
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODTruth ExpressionEvaluationLib RecoToolInterfaces AthenaKernel xAODAssociations xAODEventInfo xAODJet xAODTracking GaudiKernel MCTruthClassifierLib ReweightUtilsLib TrkExUtils DerivationFrameworkSUSYLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

