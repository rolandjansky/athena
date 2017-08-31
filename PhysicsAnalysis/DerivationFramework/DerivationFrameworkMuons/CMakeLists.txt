################################################################################
# Package: DerivationFrameworkMuons
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkMuons )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigMuonMatching
                          Reconstruction/MuonIdentification/ICaloTrkMuIdTools
                          Reconstruction/MuonIdentification/CaloTrkMuIdTools
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkMuonsLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkMuons
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloEvent CaloGeoHelpers AthenaBaseComps xAODCaloEvent xAODMuon GaudiKernel RecoToolInterfaces ExpressionEvaluationLib TrigDecisionToolLib TrigMuonMatchingLib ICaloTrkMuIdTools CaloTrkMuIdTools TrkExInterfaces
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel xAODEventInfo xAODTracking xAODTruth )

atlas_add_component( DerivationFrameworkMuons
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloGeoHelpers AthenaBaseComps xAODCaloEvent xAODMuon GaudiKernel ExpressionEvaluationLib RecoToolInterfaces TrigDecisionToolLib TrigMuonMatchingLib AthenaKernel xAODEventInfo xAODTracking xAODTruth DerivationFrameworkMuonsLib ICaloTrkMuIdTools CaloTrkMuIdTools TrkExInterfaces)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

