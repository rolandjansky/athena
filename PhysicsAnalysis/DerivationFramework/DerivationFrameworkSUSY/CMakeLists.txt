################################################################################
# Package: DerivationFrameworkSUSY
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkSUSY )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Reconstruction/RecoTools/RecoToolInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODAssociations
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/MCTruthClassifier )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkSUSYLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkSUSY
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps xAODTruth RecoToolInterfaces ExpressionEvaluationLib MCTruthClassifierLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel xAODAssociations xAODEventInfo xAODJet xAODTracking GaudiKernel )

atlas_add_component( DerivationFrameworkSUSY
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODTruth RecoToolInterfaces AthenaKernel xAODAssociations xAODEventInfo xAODJet xAODTracking GaudiKernel ExpressionEvaluationLib MCTruthClassifierLib DerivationFrameworkSUSYLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

