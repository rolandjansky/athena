################################################################################
# Package: MuonTPTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTPTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          DataQuality/GoodRunsLists
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
                          PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceHistUtils
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigMuonMatching
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTPTools
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools GoodRunsListsLib xAODBase xAODMuon MuonEfficiencyCorrectionsLib MuonPerformanceHistUtils MuonSelectorToolsLib TrigDecisionToolLib TrigMuonMatchingLib xAODEventInfo xAODEventShape xAODJet xAODTracking xAODTruth GaudiKernel MCTruthClassifierLib )

# Install files from the package:
atlas_install_headers( MuonTPTools )

