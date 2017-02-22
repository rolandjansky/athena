################################################################################
# Package: MuonTPTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTPTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   DataQuality/GoodRunsLists
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMuon
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   PhysicsAnalysis/AnalysisCommon/FsrUtils
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
   PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceHistUtils
   PhysicsAnalysis/MuonID/MuonSelectorTools
   Reconstruction/MuonIdentification/ICaloTrkMuIdTools
   Reconstruction/RecoTools/RecoToolInterfaces
   Tracking/TrkEvent/TrkParameters
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigAnalysis/TrigMuonMatching
   Event/xAOD/xAODBPhys
   PRIVATE
   Calorimeter/CaloEvent
   Control/AthContainers
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventShape
   Event/xAOD/xAODHIEvent
   Event/xAOD/xAODJet
   Event/xAOD/xAODTruth
   GaudiKernel
   PhysicsAnalysis/MCTruthClassifier
   Reconstruction/MuonIdentification/muonEvent
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkExtrapolation/TrkExInterfaces )
 
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Physics )

# Component(s) in the package:
atlas_add_component( MuonTPTools
   MuonTPTools/*.h src/*.cxx Root/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools GoodRunsListsLib xAODBase
   xAODMuon xAODPrimitives xAODTracking IsolationSelectionLib
   JetSelectorToolsLib MuonEfficiencyCorrectionsLib MuonPerformanceHistUtils
   MuonSelectorToolsLib ICaloTrkMuIdTools RecoToolInterfaces TrkParameters
   TrigDecisionToolLib TrigMuonMatchingLib CaloEvent AthContainers xAODCore
   xAODEventInfo xAODEventShape xAODHIEvent xAODJet xAODTruth GaudiKernel
   MCTruthClassifierLib muonEvent TrkSurfaces TrkExInterfaces xAODBPhys )

# Install files from the package:
atlas_install_headers( MuonTPTools )
